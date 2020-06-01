// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "InvadedPCManager.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AInvadedPCManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AInvadedPCManager();
	virtual void BeginPlay() override;
	void PlayFightAnim(class UCameraComponent* Camera,class AInvadedPlayer* PlayerValue, class AInvadedCharacter* EnemyValue);
	void ReturnToStartLocation(class UCameraComponent* Camera);
	void PlayReverseAnim();

	
private:
	class UTimelineComponent* MyTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	class UCurveFloat* fCurve;

	UPROPERTY()
	FVector StartLocation;
	
	UPROPERTY()
	FVector EndLocation;

	UPROPERTY()
	FRotator StartRotation;
	
	UPROPERTY()
	FRotator EndRotation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	float YOffset;

	FOnTimelineFloat InterpFunction;

	FOnTimelineEvent TimelineFinished;

	UFUNCTION()
		void TimelineFloatReturn(float Value);

	UFUNCTION()
		void OnTimelineFinished();
	
	class UCameraComponent* CurrentCamera;

	class AInvadedPlayer* Player;

	bool bReverse = false;
	
};
