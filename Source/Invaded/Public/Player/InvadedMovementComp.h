// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InvadedAnimInstance.h"
#include "InvadedMovementComp.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UInvadedMovementComp : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
		UInvadedMovementComp();
		void SetCharacterLocation(FVector Location, FRotator Rotation);
		UFUNCTION(Server, Reliable, WithValidation)
		void SetCharacterLocationServer(FVector Location,FRotator Rotation);
		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void SetCharacterLocationMulticast(FVector Location, FRotator Rotation);
		UFUNCTION(BlueprintCallable)
		void SetMoveStatus(EMoveStatus Status);
		UFUNCTION(BlueprintCallable,Server, Reliable, WithValidation)
		void SetMoveStatusServer(EMoveStatus Status);
		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void SetMoveStatusMulticast(EMoveStatus Status);
		UFUNCTION()
		void MoveToDestination();
		UFUNCTION(BlueprintCallable)
		void EndFight();
		UFUNCTION()
		void ReachedDestination();
		UFUNCTION()
		void SetDestination(FVector Destination);
		UFUNCTION()
		void Turn(float Rotation);
		UFUNCTION(Server,Reliable, WithValidation)
		void TurnServer(float Rotation);
		UFUNCTION(NetMulticast,Reliable, WithValidation)
		void TurnMulticast(float Rotation);
		UFUNCTION(BlueprintPure)
		bool IsMoving()  { return bIsMoving; }
		UFUNCTION(BlueprintCallable)
		bool DoTurnOnly() const { return bTurnOnly; }

		UFUNCTION(Server, Reliable, WithValidation)
		void SetIsMovingServer(bool Value);
		UFUNCTION(Server, Reliable, WithValidation)
		void SetIsMovingMulticast(bool Value);
		UFUNCTION(BlueprintCallable)
		void SetIsMoving(bool Value);
		UFUNCTION(BlueprintCallable)
		void SetTurnOnly(bool Value);

		UFUNCTION(Server, Reliable, WithValidation)
		void SetCurrentDestinationServer(FVector Value);

		void SetCurrentDestination(FVector Value);

		UFUNCTION(BlueprintCallable)
		void PlayMontage(EMontageType  Montage);
		UFUNCTION(Server,Reliable,WithValidation)
		void PlayMontageServer(EMontageType  Montage);
		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void PlayMontageMulticast(EMontageType  Montage);

		
		UFUNCTION(BlueprintCallable)
			void Rotate(float Rotation);
		UFUNCTION(Server,Reliable,WithValidation)
			void RotateServer(float Rotation);
		UFUNCTION(NetMulticast, Reliable, WithValidation)
			void RotateMulticast(float Rotation);
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,FActorComponentTickFunction * ThisTickFunction) override;

	UFUNCTION()
	bool IsNearlyEqual(FVector Vector1, FVector Vector2);

	
private:
	class UInvadedAnimInstance* AnimInstance;
	UPROPERTY(Replicated)
	FVector CurrentDestination;
	

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess="true"))
	float InterpSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float ErrorMargin;

	UPROPERTY(Replicated)
	bool bIsMoving;
	bool bTurnOnly;

	FTimerHandle MoveHandle;

	
};
