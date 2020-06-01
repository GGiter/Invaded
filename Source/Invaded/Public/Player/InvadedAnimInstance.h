// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "InvadedAnimInterface.h"
#include "InvadedAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETurnMode : uint8
{
	TM_None UMETA(DisplayName="None"),
	TM_LeftTurn 	UMETA(DisplayName = "LeftTurn"),
	TM_RightTurn 	UMETA(DisplayName = "RightTurn"),
	TM_BackwardTurn UMETA(DisplayName = "BackwardTurn")
	
};
UENUM(BlueprintType)
enum class EMoveStatus : uint8
{
	MS_Idle UMETA(DisplayName = "Idle"),
	MS_Rotating UMETA(DisplayName="Rotating"),
	MS_Walking UMETA(DisplayName = "Walking")
};
UENUM(BlueprintType)
enum class EMontageType : uint8
{
	MT_Hit UMETA(DisplayName = "Hit"),
	MT_Death UMETA(DisplayName = "Death"),
	MT_Win UMETA(DisplayName = "Win"),
	MT_ExitFight UMETA(DisplayName="ExitFight")
};
UCLASS()
class INVADED_API UInvadedAnimInstance : public UAnimInstance, public IInvadedAnimInterface
{
	GENERATED_BODY()
	
public:
	UInvadedAnimInstance();
	UFUNCTION(BlueprintCallable)
	void SetIsHit(const bool& Value);
	UFUNCTION(BlueprintCallable)
	void SetIsShooting(const bool& Value);
	void SetHaveWon(const bool& Value);
	UFUNCTION(BlueprintCallable)
	void SetIsDead(const bool& Value);
	UFUNCTION(BlueprintCallable)
	void SetTurnMode(ETurnMode Mode);
	UFUNCTION(BlueprintPure)
		EMoveStatus GetMoveStatus() const { return MoveStatus; }

	void SetMoveStatus(EMoveStatus Value);
	UFUNCTION(BlueprintCallable)
		void PlayHitMontage();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_PlayHitMontage();
	UFUNCTION(BlueprintCallable)
		void PlayDeathMontage();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_PlayDeathMontage();
	UFUNCTION(BlueprintCallable)
		void PlayWinMontage();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_PlayWinMontage();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void BP_EnterFightMode();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void BP_ExitFightMode();
private:
	UPROPERTY(BlueprintReadWrite, Category = PlayerMovement, meta = (AllowPrivateAccess = "true"))
	ETurnMode TurnMode=ETurnMode::TM_None;
	UPROPERTY(BlueprintReadWrite, Category = PlayerMovement,meta=(AllowPrivateAccess="true"))
	EMoveStatus MoveStatus = EMoveStatus::MS_Idle;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bHit=false;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bShooting=false;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bWon=false;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDead=false;
	
};
