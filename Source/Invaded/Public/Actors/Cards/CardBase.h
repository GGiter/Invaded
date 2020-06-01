// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InvadedCharacter.h"
#include "CardBase.generated.h"


USTRUCT(BlueprintType)
struct FCardInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
		FString CardName;
	UPROPERTY(BlueprintReadWrite)
		UTexture2D* Texture;
};

UCLASS()
class INVADED_API ACardBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACardBase();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void OnInteract(AInvadedCharacter* Caller);
	virtual void OnInteract_Implementation(AInvadedCharacter* Caller);
	UFUNCTION(BlueprintCallable, Category = "Config|Description")
		FString GetCardName() const { return CardName; }
	UFUNCTION(BlueprintCallable, Category = "Config|Description")
		FString GetDesription() const { return Description; }
	UFUNCTION(BlueprintCallable, Category = "Config")
		UTexture2D* GetTexture() const { return TextureRef; }
	UFUNCTION(BlueprintPure, Category = "Config")
		class UMaterialInstanceDynamic* GetMaterial() { return MaterialDyn; }
	UFUNCTION(BlueprintCallable, Category = "Config")
		void SetFirstColor(FLinearColor Value);
	UFUNCTION(BlueprintCallable, Category = "Config")
		void SetSecondColor(FLinearColor Value);
	UFUNCTION(BlueprintCallable, Category = "Effects")
		void SpawnEffect(class AInvadedCharacter* Causer, class AInvadedCharacter* Target);
	UFUNCTION(BlueprintCallable, Category = "Effects")
		virtual void Activate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target); //only server should call
protected:

	UFUNCTION(Server,Reliable,WithValidation)
		void ServerSpawnEffect(class AInvadedCharacter* Causer, class AInvadedCharacter* Target);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastSpawnEffect(class AInvadedCharacter* Causer, class AInvadedCharacter* Target);

	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
		void BP_SpawnEffect(class AInvadedCharacter* Causer, class AInvadedCharacter* Target);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly,Category="Config|Description")
	FString CardName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Description")
	FString Description;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* TextureRef;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	class UMaterialInterface* MaterialRef;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FLinearColor FirstColor = { 1.0f,0.306f,0.487f,1.0f };
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FLinearColor SecondColor = { 0.0719f,1.0f,0.0581f,1.0f };

	class  UMaterialInstanceDynamic* MaterialDyn;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
