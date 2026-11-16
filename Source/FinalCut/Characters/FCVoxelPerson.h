// FINAL CUT - a voxel character: beveled blocks riding on the (invisible) mannequin skeleton.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FCVoxelPerson.generated.h"

class UBlendSpace;
class UFCVoxelBodyData;
class UMaterialInterface;
class USkeletalMeshComponent;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * Rook, the Warden, guards ... The actor faces +X with its pivot at the feet (same as the old
 * BP_VoxelPerson), so Level Sequences can keep keyframing the actor transform. The walk / idle
 * animation is picked from how fast the actor is being moved.
 */
UCLASS(Blueprintable)
class FINALCUT_API AFCVoxelPerson : public AActor
{
	GENERATED_BODY()

public:
	AFCVoxelPerson();

	/** Which body to wear (a data asset in /Game/Characters/Bodies). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Person")
	TObjectPtr<UFCVoxelBodyData> Body;

	/** Actor speed (cm/s) is multiplied by this before it picks the animation: higher = livelier steps. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Person")
	float AnimSpeedScale = 1.8f;

	/** Upper limit for the animation speed value (keeps sequence walks from turning into a jog). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Person")
	float MaxAnimSpeed = 260.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voxel Person")
	TObjectPtr<USkeletalMeshComponent> Skeleton;

	/** Throws the blocks away and builds them again from the Body data. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Voxel Person")
	void RebuildBody();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostRegisterAllComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<UBlendSpace> Locomotion;

	UPROPERTY()
	TObjectPtr<UStaticMesh> BlockMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BlockMaterial;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> Blocks;

	FVector LastLocation = FVector::ZeroVector;
	float Speed = 0.0f;
};
