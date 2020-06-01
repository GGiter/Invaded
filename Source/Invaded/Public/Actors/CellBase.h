// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridGeneratorAlg.h"
#include "Interfaces/HighlightInterface.h"
#include "CellBase.generated.h"




UCLASS()
class INVADED_API ACellBase : public AActor , public IHighlightInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACellBase();

	UStaticMeshComponent* GetMesh()
	{
		return S_Mesh;
	}

	//A*Star alg
	uint16 GetFCost(){ return GCost + HCost; }
	uint16 GetHCost() const { return HCost; }
	uint16 GetGCost() const { return GCost; }
	
	void SetGCost(uint16 Cost);
	
	void SetHCost(uint16 Cost);
	
	void SetParent(ACellBase* Cell);

	int16 CompareCells(ACellBase* Cell);

	ACellBase* GetParent() const { return Parent; }

	virtual void Activate(APawn*Caller);

	//HighlightInterface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void OnInteract(APawn* Caller);
	virtual void OnInteract_Implementation(APawn*Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Highlight")
	void SetHighlightState(EHighlightState Value);
	virtual void SetHighlightState_Implementation(EHighlightState Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Highlight")
	void OnBeginFocus();
	virtual void OnBeginFocus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Highlight")
	void OnEndFocus();
	virtual void OnEndFocus_Implementation();

	void UpdateCellMaterial(const FLinearColor& BaseColorValue, EHighlightState HighlightState);

	void SetBaseColor(const FLinearColor& Value);

	bool GetIsWalkable() const { return bWalkable; }

	void SetIsWalkable(bool Value);

	bool IsFocusable() const { return bFocusable; }

	void SetIsFocusable(bool Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetCellType(ECellType Value);

	UFUNCTION(BlueprintPure)
	ECellType GetCellType() const { return CellType; }

	void SetIsOccupied(bool Value);
	void SetOccupyingPlayer(class APawn* Player);

	UFUNCTION(BlueprintPure)
	bool GetIsOccupied() const { return bIsOccupied; }

	UFUNCTION(BlueprintPure)
	class APawn* GetOccupyingPlayer() const { return OccupyingPlayer; }

	//GridGenerator
	virtual FVector GetActorLocationTo(AActor* Actor);

	//Flags
	TArray<class AFlag*> GetFlags()
	{
		return Flags;
	};

	void AddFlag(class AFlag* Flag);

	void RemoveFlag(class AFlag* Flag);

	void RemoveFlags();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float CalculateRotation(AActor* Caller);

	void UpdateCellState();

protected:
	//A*Star alg
	uint16 GCost;

	uint16 HCost;

	ACellBase* Parent;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
		UStaticMeshComponent* S_Mesh;

	UPROPERTY(EditDefaultsOnly,Replicated)
		bool bWalkable =true;

	UPROPERTY(EditDefaultsOnly, Replicated)
		bool bFocusable = true;

	//Highlight
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		class UMaterialInterface* TileMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FLinearColor PositiveGlowColor;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FLinearColor NegativeGlowColor;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FLinearColor SpecialGlowColor;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FLinearColor InactiveColor;
	UPROPERTY(EditDefaultsOnly, Category = "Material",Replicated)
		FLinearColor BaseColor;

	class UMaterialInstanceDynamic* DynamicTileMaterial;

	UPROPERTY(Replicated)
		ECellType CellType;
	UPROPERTY(Replicated)
		bool bIsOccupied;
	UPROPERTY(Replicated)
		class APawn* OccupyingPlayer;
	UPROPERTY(Replicated)
		TArray<class AFlag*> Flags;


	
};
