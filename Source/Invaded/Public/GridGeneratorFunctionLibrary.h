// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridGenerator.h"
#include "GridGeneratorCSVAlg.h"
#include "GridGenerator.h"
#include "Components/ActorComponent.h"
#include "GridGeneratorFunctionLibrary.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGridBuildData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FTransform> Transforms;
	UPROPERTY(BlueprintReadWrite)
		TArray<ECellType> CellTypes;

	FGridBuildData()
	{
	}
	FGridBuildData(TArray<FTransform> Transforms, TArray<ECellType> CellTypes)
	{
		this->Transforms = Transforms;
		this->CellTypes = CellTypes;
	}
};

//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
UCLASS()
class INVADED_API AGridGeneratorFunctionLibrary : public AActor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void GenerateBuildData(FGridGeneratorInfo GridInfo, float Scale);
	UFUNCTION(BlueprintCallable)
	void DestroyCells();

	UFUNCTION(BlueprintPure)
	TArray<FString> FindMaps();
protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FGridBuildData GetBuildData();

	virtual void Tick(float DeltaTime) override;

	void BuildData(FGridBuildData BuildData);

	FGridGeneratorInfo* GridConfigRef;
	float Scale;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellBase> CellBaseClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellAttack> CellAttackClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellDefense> CellDefenseClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellCorner> CellCornerClass;

	UPROPERTY(BlueprintReadWrite)
	TArray<class ACellBase*> CellArray;
};
