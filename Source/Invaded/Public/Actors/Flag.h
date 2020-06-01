// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flag.generated.h"

UCLASS()
class INVADED_API AFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlag();
	void SetOwnerName(FName Value);
	FName GetOwnerName() const { return OwnerName; }

	UFUNCTION(Server,Reliable,WithValidation)
	void DestroyFlag(class AInvadedCharacter* Player);

	void SetFlagTextureIndex(const int32& Value);

	void SetFirstColor(FLinearColor Value);
	
	void SetSecondColor(FLinearColor Value);

	void RemoveFromPlayer();
	UFUNCTION(Server, Reliable, WithValidation)
	void RemoveFromPlayerServer();
	
	void SetDeathCell(class ACellBase* Value);

	void SetCornerCell(class ACellBase* Value);

	void SetCurrentOwner(class AInvadedCharacter* Value);

	void SetFirstOwner(class AInvadedCharacter* Value);

	void FinishSetup();

	class ACellBase* GetLastOversteppedCell() const { return LastOversteppedCell; }
	void SetLastOversteppedCell(class ACellBase* Value);
	class ACellBase* GetDeathCell() const { return DeathCell; }
	class AInvadedCharacter* GetFirstOwner() const { return FirstOwner; }
	class ACellBase* GetCornerCell() const { return CornerCell; }
protected:

	UFUNCTION()
		void OnRep_FlagTextureIndex();

	UPROPERTY(ReplicatedUsing = OnRep_FlagTextureIndex)
		bool DoRep_FlagTextureIndex;

	UPROPERTY(Replicated)
		int32 FlagTextureIndex;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void RemoveFlagTick();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void UpdateMaterial();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* SK_Mesh;
	
	UPROPERTY(EditAnywhere,Replicated)
	FName OwnerName;
	UPROPERTY(Replicated)
	FLinearColor FirstColor;
	UPROPERTY(Replicated)
	FLinearColor SecondColor;
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* Material;
	UPROPERTY(EditDefaultsOnly)
	TArray<UTexture*> FlagTextureArray;
	UPROPERTY(EditDefaultsOnly)
	TArray<UTexture*> FlagTextureArray2;
	UPROPERTY(Replicated)
	class AInvadedCharacter* CurrentOwner;
	UPROPERTY(Replicated)
	class AInvadedCharacter* FirstOwner;
	UPROPERTY(Replicated)
	class ACellBase* LastOversteppedCell;
	UPROPERTY(Replicated)
	class ACellBase* DeathCell;
	UPROPERTY(Replicated)
	class ACellBase* CornerCell;

	class UMaterialInstanceDynamic* DynMaterial;

	FTimerHandle RemoveFlagHandle;
	
};
