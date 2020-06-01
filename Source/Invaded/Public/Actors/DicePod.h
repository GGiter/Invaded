// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DicePod.generated.h"

UCLASS()
class INVADED_API ADicePod : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADicePod();
	void SpawnDice(const bool AdditionalThrow);
	void SpawnDiceSpecified(class UInvadedData* SpecifiedPlayerValue);
	void Toggle();
	class ADice* GetDice() { return Dice; }

	void SetDiceValue(uint8 Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetDiceValueServer(uint8 Value);

	void SetFirstColor(FLinearColor Value);

	void SetSecondColor(FLinearColor Value);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:

	void ProjectCameraCurve();

	void EndCameraCurve();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ToggleDicePod();

	UFUNCTION(Server, Reliable, WithValidation)
		void ToggleDicePodServer();

	void EndDice(bool RepeatThrow);

	UFUNCTION(Server, Reliable, WithValidation)
		void EndDiceServer(bool RepeatThrow);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void UpdateMaterial();
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USceneComponent* Dummy;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
		class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USceneCaptureComponent2D* SceneCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SM_Mesh;
	UPROPERTY(EditDefaultsOnly)
		float DiceThrowHeight = 500.0f;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ADice> DiceClass;
	UPROPERTY(EditDefaultsOnly)
		float LocationSpeed;
	UPROPERTY(EditDefaultsOnly)
		float DistanceFromDice;
	UPROPERTY(EditDefaultsOnly)
		float RotationSpeed;
	UPROPERTY(EditDefaultsOnly)
		float CurveDelay;

	UPROPERTY(EditDefaultsOnly)
		FVector Force = { 0.5f,0.5f,-1.0f };

	float RotationYaw;
	FVector StartLocation;
	FRotator StartRotation;
	

	bool bProjectCamera;

	UPROPERTY(BlueprintReadWrite)
		class ADice* Dice;
	UPROPERTY(BlueprintReadWrite)
		bool bRotateCamera;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* Material;

	class UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY(Replicated)
		class UInvadedData* CurrentOwner;
	UPROPERTY(Replicated)
		FRotator NewRotator;
	UPROPERTY(Replicated)
		FQuat NewQuat;
	UPROPERTY(Replicated)
		uint8 DiceValue;
	UPROPERTY(Replicated)
		FLinearColor FirstColor;
	UPROPERTY(Replicated)
		FLinearColor SecondColor;

	float AlphaValue;
	bool bAdditionalThrow;

	FTimerHandle CameraHandle;

	class UInvadedData* SpecifiedPlayer;
	
};
