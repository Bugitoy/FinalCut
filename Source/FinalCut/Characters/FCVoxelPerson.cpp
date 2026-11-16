#include "Characters/FCVoxelPerson.h"

#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/BlendSpace.h"
#include "AnimationRuntime.h"
#include "Characters/FCVoxelBodyData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FName VoxelBlockTag(TEXT("VoxelBlock"));
}

AFCVoxelPerson::AFCVoxelPerson()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// The mannequin is only the invisible "bones" that the blocks ride on.
	Skeleton = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeleton"));
	Skeleton->SetupAttachment(Root);
	Skeleton->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));   // mannequin faces +Y, the actor faces +X
	Skeleton->SetCollisionProfileName(TEXT("NoCollision"));
	Skeleton->SetVisibility(false, false);
	Skeleton->SetCastShadow(false);
	Skeleton->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	Skeleton->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (MeshFinder.Succeeded())
	{
		Skeleton->SetSkeletalMeshAsset(MeshFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UBlendSpace> LocomotionFinder(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/BS_Idle_Walk_Run.BS_Idle_Walk_Run"));
	if (LocomotionFinder.Succeeded())
	{
		Locomotion = LocomotionFinder.Object;
		Skeleton->AnimationData.AnimToPlay = Locomotion;
		Skeleton->AnimationData.bSavedLooping = true;
		Skeleton->AnimationData.bSavedPlaying = true;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BlockFinder(
		TEXT("/Game/Voxel/LabProps/SM_BevelCube/StaticMeshes/SM_BevelCube.SM_BevelCube"));
	if (BlockFinder.Succeeded())
	{
		BlockMesh = BlockFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(
		TEXT("/Game/Voxel/Materials/M_VoxelPerson.M_VoxelPerson"));
	if (MaterialFinder.Succeeded())
	{
		BlockMaterial = MaterialFinder.Object;
	}
}

void AFCVoxelPerson::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RebuildBody();
}

void AFCVoxelPerson::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
	// The blocks are never saved with the level; build them whenever the actor (re)appears.
	if (Blocks.Num() == 0 && !IsTemplate())
	{
		RebuildBody();
	}
}

void AFCVoxelPerson::RebuildBody()
{
	// Remove every old block, including copies that came along when the level was duplicated for Play.
	TInlineComponentArray<UStaticMeshComponent*> Old(this);
	for (UStaticMeshComponent* Component : Old)
	{
		if (Component && Component->ComponentHasTag(VoxelBlockTag))
		{
			Component->DestroyComponent();
		}
	}
	Blocks.Reset();

	USkeletalMesh* Mesh = Skeleton ? Skeleton->GetSkeletalMeshAsset() : nullptr;
	if (!Body || !Mesh || !BlockMesh || !Skeleton->IsRegistered())
	{
		return;
	}
	Skeleton->SetRelativeScale3D(FVector(Body->BodyScale));

	const FReferenceSkeleton& Ref = Mesh->GetRefSkeleton();
	const FQuat CharToMesh = FRotator(0.0, 90.0, 0.0).Quaternion();   // character X-forward -> mesh Y-forward

	for (const FFCVoxelPart& Part : Body->Parts)
	{
		const int32 BoneIndex = Ref.FindBoneIndex(Part.Bone);
		if (BoneIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("[people] %s: part %s names unknown bone %s"),
				*GetName(), *Part.Name.ToString(), *Part.Bone.ToString());
			continue;
		}
		const FTransform BoneRef = FAnimationRuntime::GetComponentSpaceTransformRefPose(Ref, BoneIndex);
		const int32 ToIndex = Part.ToBone.IsNone() ? INDEX_NONE : Ref.FindBoneIndex(Part.ToBone);

		FTransform PartRef;   // where the block sits in the reference pose (mesh space)
		if (ToIndex != INDEX_NONE)
		{
			const FVector A = BoneRef.GetLocation();
			const FVector B = FAnimationRuntime::GetComponentSpaceTransformRefPose(Ref, ToIndex).GetLocation();
			const FVector Dir = (B - A).GetSafeNormal();
			FVector Forward(0.0, 1.0, 0.0);
			if (FMath::Abs(Dir | Forward) > 0.8)
			{
				Forward = FVector(0.0, 0.0, 1.0);
			}
			const FQuat LimbRot = FRotationMatrix::MakeFromXZ(Dir, Forward).ToQuat();
			PartRef = FTransform(
				LimbRot * Part.Rotation.Quaternion(),
				(A + B) * 0.5 + LimbRot.RotateVector(Part.Offset),
				FVector((B - A).Size() + Part.Size.X, Part.Size.Y, Part.Size.Z) / 100.0);
		}
		else
		{
			PartRef = FTransform(
				CharToMesh * Part.Rotation.Quaternion(),
				BoneRef.GetLocation() + CharToMesh.RotateVector(Part.Offset),
				Part.Size / 100.0);
		}

		const FName ComponentName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), Part.Name);
		UStaticMeshComponent* Block = NewObject<UStaticMeshComponent>(this, ComponentName, RF_Transient);
		Block->ComponentTags.Add(VoxelBlockTag);
		Block->SetStaticMesh(BlockMesh);
		if (BlockMaterial)
		{
			Block->SetMaterial(0, BlockMaterial);
		}
		Block->SetMobility(EComponentMobility::Movable);
		Block->SetCollisionProfileName(TEXT("NoCollision"));
		Block->SetCanEverAffectNavigation(false);
		Block->SetDefaultCustomPrimitiveDataVector4(0, FVector4(Part.Color.R, Part.Color.G, Part.Color.B, 1.0));
		Block->SetupAttachment(Skeleton, Part.Bone);
		Block->SetRelativeTransform(PartRef.GetRelativeTransform(BoneRef));
		Block->RegisterComponent();
		Blocks.Add(Block);
	}
}

void AFCVoxelPerson::BeginPlay()
{
	Super::BeginPlay();
	LastLocation = GetActorLocation();
	if (Locomotion && Skeleton)
	{
		Skeleton->PlayAnimation(Locomotion, true);
	}
}

void AFCVoxelPerson::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FVector Location = GetActorLocation();
	FVector Moved = Location - LastLocation;
	Moved.Z = 0.0;
	LastLocation = Location;

	float RawSpeed = DeltaSeconds > KINDA_SMALL_NUMBER ? Moved.Size() / DeltaSeconds : 0.0f;
	if (RawSpeed > 1500.0f)
	{
		RawSpeed = 0.0f;   // a cut or teleport, not a sprint
	}
	Speed = FMath::FInterpTo(Speed, RawSpeed, DeltaSeconds, 8.0f);

	if (UAnimSingleNodeInstance* Anim = Skeleton ? Skeleton->GetSingleNodeInstance() : nullptr)
	{
		// BS_Idle_Walk_Run: X = direction (0 = forward), Y = speed.
		Anim->SetBlendSpacePosition(FVector(0.0, FMath::Min(Speed * AnimSpeedScale, MaxAnimSpeed), 0.0));
	}
}
