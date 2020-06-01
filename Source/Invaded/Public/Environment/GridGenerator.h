// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridGeneratorAlg.h"
#include "Alg/RegenerateGrid.h"
#include "SaveGames/LobbySG.h"
#include "GridGenerator.generated.h"



UCLASS()
class INVADED_API AGridGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridGenerator();
	UFUNCTION(BlueprintCallable,Server,Reliable,WithValidation)
	void Generate();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void SetMapScale(float Value);

	UFUNCTION(NetMulticast,Reliable,WithValidation)
	void SetMapScaleMulticast(float Value);

	void SetMapInfo(const FMapInfo& Value);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	TArray<class ACellBase*> GetNeighbours(class ACellBase* Cell);

	TArray<class ACellCorner*> GetCornerCells() const { return CellCornerArray; }

	void SetPlayerStart();

	void SpawnFlags();

	void ResetFlags();

	void RegenerateSpecialCells();

	void RegenerateSpecialCellsAsync();

	void DisableQuadrant(class ACellBase* StartingCell,class AInvadedCharacter* Causer);

	void DisableQuadrantAsync(ACellBase * StartingCell, class AInvadedCharacter* Causer);

	static float GetMapScale() { return MapScale; }

	UPROPERTY(Replicated)
		TArray<class ACellBase*> CellBaseArray;
	UPROPERTY(Replicated)
		TArray<FVector2D> CellBaseLocation;
	UPROPERTY(Replicated)
		TArray<class ACellBase*> CellDisabledArray;
	UPROPERTY(Replicated)
		TArray<FVector> CellDisabledLocations;
	UPROPERTY(Replicated)
		TArray<class ACellAttack*> CellAttackArray;
	UPROPERTY(Replicated)
		TArray<class ACellDefense*> CellDefenseArray;
	UPROPERTY(Replicated)
		TArray<class ACellCorner*> CellCornerArray;
	UPROPERTY(Replicated)
		TArray<class ACellBase*> StartingCellArray;
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<class ACellBase*> CellArray;
	UPROPERTY(Replicated)
		TArray<int32> DisabledQuadrants;
private:

	
	static float MapScale;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* GridStart;

	UFUNCTION(Server,Reliable,WithValidation)
		void UpgradeGridConfigServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void SetGenerate(bool Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetRegenerate(bool Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void GenerationFinished();

	UFUNCTION(Server, Reliable, WithValidation)
		void RegenerationFinished();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void SetActorLocation(FVector Location, AActor* Actor);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ScalePlayer(class AInvadedCharacter* Player,float Scale);
	
	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellBase> CellBaseClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellAttack> CellAttackClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellDefense> CellDefenseClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellCorner> CellCornerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cells")
		TSubclassOf<class ACellBase> CellHiddenClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Config|Flag")
		TSubclassOf<class AFlag> FlagClass;

	UPROPERTY(Replicated)
		bool bGenerate = true;
	UPROPERTY(Replicated)
		bool bRegenerate = true;
	UPROPERTY(Replicated)
		FMapInfo MapInfo;

	UPROPERTY(EditDefaultsOnly,Replicated)
		 FGridGeneratorInfo GridConfig;
	
	void GenerateAttackCells(FGridGeneratorInfo* Data);

	void GenerateDefenseCells(FGridGeneratorInfo* Data);

	bool ClearNeighbourhood(FGridGeneratorInfo* Data, uint8 Index, uint8 Range, ECellType CellType);

	TArray<uint8> GetNeighboursIndex(FGridGeneratorInfo* Data, uint8 Index, bool bOnlyBlank);

	FGridGeneratorInfo* GridConfigRef;

	FGridRegeneratorInfo* RegenerateConfigRef;

	FTimerHandle PlayerStartHandle;
	
};
