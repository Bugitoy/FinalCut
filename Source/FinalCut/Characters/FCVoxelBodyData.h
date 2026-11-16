// FINAL CUT - data that describes one voxel character body (DESIGN.md 4.2).
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FCVoxelBodyData.generated.h"

/**
 * One beveled block of a character. Every block follows one bone of the mannequin skeleton, so the
 * body bends at real joints and no limb can drift away from the body.
 *
 * Two kinds of block:
 *  - LIMB (ToBone set): the block spans the joint "Bone" to the joint "ToBone" (thigh, shin, arm ...).
 *      Size   = (extra length, width, depth) in cm.   Offset = (along the limb, sideways, forward) in cm.
 *  - BOX  (ToBone empty): a block placed relative to the joint "Bone" in character space
 *      (X = forward, Y = right, Z = up).   Size = (depth, width, height) in cm.   Offset in cm.
 */
USTRUCT(BlueprintType)
struct FFCVoxelPart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FName Bone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FName ToBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FVector Size = FVector(10.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FLinearColor Color = FLinearColor::White;
};

/** A complete body: Rook, Warden Hale, Officer Okafor ... Created by Content/Python/build_people.py. */
UCLASS(BlueprintType)
class FINALCUT_API UFCVoxelBodyData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body")
	TArray<FFCVoxelPart> Parts;

	/** Overall size of the character (1 = mannequin height, about 183 cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body")
	float BodyScale = 1.0f;
};
