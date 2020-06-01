// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedMovementComp.h"
#include "InvadedAnimInstance.h"
#include "InvadedPlayer.h"
#include "InvadedData.h"
#include "InvadedCharacter.h"
#include "UnrealNetwork.h"
#include "TimerManager.h"
#include "GameModes/InvadedGS.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "InvadedAIPlayer.h"
#include "Actors/CellBase.h"
#include "InvadedGM.h"
#include "..\..\Public\Player\InvadedMovementComp.h"


UInvadedMovementComp::UInvadedMovementComp()
{
	bReplicates = true;
	InterpSpeed = 90.0f;
	ErrorMargin =0.0f;
	bIsMoving = false;
	bTurnOnly = false;

	
}
void UInvadedMovementComp::SetCharacterLocation(FVector Location, FRotator Rotation)
{
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	
	if (GetOwnerRole() < ROLE_Authority&&!InvadedCharacter->IsA<AInvadedAIPlayer>())
	{
		SetCharacterLocationServer(Location, Rotation);
	}
	else
	{
		SetCharacterLocationMulticast(Location, Rotation);

	}
	
	//if (GetOwnerRole() == ROLE_Authority)
	//	GetOwner()->SetActorLocation(Location);
}
void UInvadedMovementComp::SetCharacterLocationServer_Implementation(FVector Location, FRotator Rotation)
{
	SetCharacterLocationMulticast(Location, Rotation);
}
bool UInvadedMovementComp::SetCharacterLocationServer_Validate(FVector Location, FRotator Rotation)
{
	return true;
}
void UInvadedMovementComp::SetCharacterLocationMulticast_Implementation(FVector Location, FRotator Rotation)
{
	//UE_LOG(LogTemp, Warning, TEXT("IS MoVING %s"),*Location.ToString());
	GetOwner()->TeleportTo(FVector(Location.X,Location.Y,GetOwner()->GetActorLocation().Z), Rotation);
}
bool UInvadedMovementComp::SetCharacterLocationMulticast_Validate(FVector Location, FRotator Rotation)
{
	return true;
}
void UInvadedMovementComp::SetMoveStatus(EMoveStatus Status)
{
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	if (GetOwnerRole() < ROLE_Authority&&InvadedCharacter->IsLocallyControlled())
	{
		SetMoveStatusServer(Status);
	}
	else
	{
		AInvadedCharacter* PlayerRef = Cast<AInvadedCharacter>(GetOwner());
		if (PlayerRef)
		{
			UInvadedData* InvadedPS = PlayerRef->GetInvadedPS();
			if (InvadedPS)
			{
				InvadedPS->SetMoveStatus(Status);
			}
		}
		SetMoveStatusMulticast(Status);
	}
}
void UInvadedMovementComp::SetMoveStatusServer_Implementation(EMoveStatus Status)
{
	SetMoveStatus(Status);
	
}
bool UInvadedMovementComp::SetMoveStatusServer_Validate(EMoveStatus Status)
{
	return true;
}
void UInvadedMovementComp::SetMoveStatusMulticast_Implementation(EMoveStatus Status)
{
	AInvadedCharacter* PlayerRef = Cast<AInvadedCharacter>(GetOwner());
	if (PlayerRef)
	{
		UInvadedData* InvadedPS = PlayerRef->GetInvadedPS();
		if (InvadedPS)
		{
			InvadedPS->SetMoveStatus(Status);
		}
	}
}
bool UInvadedMovementComp::SetMoveStatusMulticast_Validate(EMoveStatus Status)
{
	return true;
}
void UInvadedMovementComp::MoveToDestination()
{

	
	AInvadedCharacter* PlayerRef = Cast<AInvadedCharacter>(GetOwner());
	if (PlayerRef)
	{
		
		
			SetCharacterLocation(FMath::VInterpConstantTo(PlayerRef->GetActorLocation(), CurrentDestination, GetWorld()->GetDeltaSeconds(), InterpSpeed*AGridGenerator::GetMapScale()), PlayerRef->GetActorRotation());
			
		

			//UE_LOG(LogTemp, Warning, TEXT("TickComponent %s %s %f"), *PlayerRef->GetActorLocation().ToString(), *CurrentDestination.ToString(), GetWorld()->GetDeltaSeconds())

			if (IsNearlyEqual(PlayerRef->GetActorLocation(),CurrentDestination))
				{
				SetCharacterLocation( CurrentDestination,PlayerRef->GetActorRotation());
					ReachedDestination();
				}
			
			
			
			
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToDestination cast fail"));
	}
}
void UInvadedMovementComp::EndFight()
{
	//bIsMoving = false;
	//SetMoveStatus(EMoveStatus::MS_Idle);
	SetTurnOnly(false);
}
void UInvadedMovementComp::ReachedDestination()
{
	
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());

	if (InvadedCharacter)
	{
		AInvadedGS* InvadedGS = Cast<AInvadedGS>(GetWorld()->GetGameState());
		UInvadedData* InvadedPS = Cast<UInvadedData>(InvadedGS->GetCurrentPlayer());
		UE_LOG(LogTemp, Warning, TEXT("PathSize %d"), InvadedCharacter->GetRPath().Num());
		UE_LOG(LogTemp, Warning, TEXT("RemainingMoves %d"), InvadedPS->GetRemainingMoves());
		if (InvadedCharacter->GetRPath().Num() > 0)
		{
			SetMoveStatus(EMoveStatus::MS_Walking);
			SetIsMoving(true);

			if (InvadedCharacter->GetOversteppedCell())
			{
				InvadedCharacter->GetOversteppedCell()->Activate(InvadedCharacter);
			}

			InvadedCharacter->NextPathCell();
		
		}
		else
		{
			SetIsMoving(false);
			SetMoveStatus(EMoveStatus::MS_Idle);

			if (InvadedCharacter->GetOversteppedCell())
			{
				InvadedCharacter->GetOversteppedCell()->Activate(InvadedCharacter);
			}
			
			InvadedCharacter->ClearSelections();
			InvadedCharacter->ClearLastRetracePath();
			if (InvadedCharacter->IsA<AInvadedPlayer>())
			{

				

				if (InvadedPS->GetRemainingMoves() < 1&&InvadedPS->HasThrownDice())
					InvadedCharacter->CallTurnEnd();
				
			}
			else
			{
				

				if (InvadedPS->GetRemainingMoves() < 1)
					InvadedCharacter->CallTurnEnd();
				else if (InvadedCharacter->IsA<AInvadedAIPlayer>())
					InvadedCharacter->CallTurnEnd(); //To do find new path
			}
			
		}
		
	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached Destination cast fail"));
	}
	

}
void UInvadedMovementComp::SetDestination(FVector Destination)
{

	bIsMoving = true;

	SetIsMoving(true);

	Destination = FVector(Destination.X, Destination.Y, GetOwner()->GetActorLocation().Z);

	

	SetCurrentDestination(Destination);

	

	ForceReplicationUpdate();
	
	//CurrentDestination = FVector(Destination.X, Destination.Y, PlayerRef->GetActorLocation().Z);
	

}
void UInvadedMovementComp::Turn(float Rotation)
{
	AInvadedCharacter* PlayerRef = Cast<AInvadedCharacter>(GetOwner());
	if (GetOwnerRole() < ROLE_Authority&&PlayerRef->IsLocallyControlled())
	{
		TurnServer(Rotation);
	}
	else
	{
		TurnMulticast(Rotation);
	}
}
void UInvadedMovementComp::SetIsMovingServer_Implementation(bool Value)
{
	SetIsMoving(Value);
}
bool UInvadedMovementComp::SetIsMovingServer_Validate(bool Value)
{
	return true;
}
void UInvadedMovementComp::SetIsMovingMulticast_Implementation(bool Value)
{
	bIsMoving = Value;
}
bool UInvadedMovementComp::SetIsMovingMulticast_Validate(bool Value)
{
	return true;
}
void UInvadedMovementComp::SetIsMoving(bool  Value)
{
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	if (GetOwnerRole() < ROLE_Authority&&InvadedCharacter->IsPlayerControlled())
	{
		SetIsMovingServer(Value);
	}
	else
	{
		if(InvadedCharacter->IsPlayerControlled())
		SetIsMovingMulticast(Value);

		bIsMoving = Value;
	}
}
void UInvadedMovementComp::SetTurnOnly(bool  Value)
{
	bTurnOnly = Value;
	//ForceReplicationUpdate();
	if (!bTurnOnly)
	{
		AInvadedAIPlayer* AIPlayer = Cast<AInvadedAIPlayer>(GetOwner());
		if (AIPlayer)
		{
			if (AIPlayer->GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_Fight)
			{
				if (GetOwnerRole() == ROLE_Authority)
				{
					if (GetWorld()->GetAuthGameMode<AInvadedGM>()->GetCurrentFightGame())
						GetWorld()->GetAuthGameMode<AInvadedGM>()->GetCurrentFightGame()->InitReached(AIPlayer->GetInvadedPS());
				}
			
				//AIPlayer->FinishFightMode();
			}
		}
	}
}
void UInvadedMovementComp::SetCurrentDestinationServer_Implementation(FVector Value)
{
	SetCurrentDestination(Value);
}
bool UInvadedMovementComp::SetCurrentDestinationServer_Validate(FVector Value)
{
	return true;
}
void UInvadedMovementComp::SetCurrentDestination(FVector Value)
{
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	if (GetOwnerRole() < ROLE_Authority&&InvadedCharacter->IsLocallyControlled())
	{
		SetCurrentDestinationServer(Value);
	}
	else
	{
		CurrentDestination = Value;
	}
}
void UInvadedMovementComp::PlayMontage(EMontageType  Montage)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		PlayMontageServer(Montage);
	}
	else
	{
		PlayMontageMulticast(Montage);
	}
}

void UInvadedMovementComp::PlayMontageServer_Implementation(EMontageType  Montage)
{
	PlayMontageMulticast(Montage);
}
bool UInvadedMovementComp::PlayMontageServer_Validate(EMontageType  Montage)
{
	return true;
}
void UInvadedMovementComp::PlayMontageMulticast_Implementation(EMontageType  Montage)
{
	AInvadedCharacter* InvadedCharacter = Cast< AInvadedCharacter>(GetOwner());

	if(InvadedCharacter)
	AnimInstance = InvadedCharacter->GetAnimInstance();
	if (AnimInstance)
	{
		switch (Montage)
		{
		case EMontageType::MT_Hit:
		{
			AnimInstance->PlayHitMontage();
			break;
		}
		case EMontageType::MT_Death:
		{
			AnimInstance->PlayDeathMontage();
			break;
		}
		case EMontageType::MT_Win:
		{
			AnimInstance->PlayWinMontage();
			break;
		}
		case EMontageType::MT_ExitFight:
		{
			AnimInstance->BP_ExitFightMode();
			break;
		}
		}
	}
	
}
bool UInvadedMovementComp::PlayMontageMulticast_Validate(EMontageType  Montage)
{
	return true;
}
void UInvadedMovementComp::Rotate(float Rotation)
{
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	if (GetOwnerRole() < ROLE_Authority&&InvadedCharacter->IsPlayerControlled())
	{
		RotateServer(Rotation);
	}
	else if (!FMath::IsNearlyEqual(GetOwner()->GetActorRotation().Yaw,Rotation,10.0f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Rotate"))
			RotateMulticast(Rotation);
	}
}
void UInvadedMovementComp::RotateServer_Implementation(float Rotation)
{
	Rotate(Rotation);
}
bool UInvadedMovementComp::RotateServer_Validate(float Rotation)
{
	return true;
}
void UInvadedMovementComp::RotateMulticast_Implementation(float Rotation)
{
	GetOwner()->SetActorRotation(FRotator(0.0f, Rotation, 0.0f));
}
bool UInvadedMovementComp::RotateMulticast_Validate(float Rotation)
{
	return true;
}
bool UInvadedMovementComp::IsNearlyEqual(FVector Vector1,  FVector  Vector2)
{
	return (FMath::IsNearlyEqual(Vector1.X, Vector2.X, ErrorMargin)) && (FMath::IsNearlyEqual(Vector1.Y, Vector2.Y, ErrorMargin));
}


void UInvadedMovementComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
		AInvadedCharacter* PlayerRef = Cast<AInvadedCharacter>(GetOwner());
		if (PlayerRef)
		{
			if (GetOwnerRole() == ROLE_Authority)
			{
				UInvadedData* InvadedPS = PlayerRef->GetInvadedPS();
				if (InvadedPS)
				{

					if (bIsMoving)
					{

						//UE_LOG(LogTemp, Warning, TEXT("bIsMoving %d"), bIsMoving);
						if (InvadedPS->GetMoveStatus() == EMoveStatus::MS_Walking)
						{
							MoveToDestination();
						}
						else if (InvadedPS->GetMoveStatus() == EMoveStatus::MS_Rotating)
						{
							//UE_LOG(LogTemp, Warning, TEXT("MoveStatus Rotating"));
						}
						else if (InvadedPS->GetMoveStatus() == EMoveStatus::MS_Idle)
						{
							//	UE_LOG(LogTemp, Warning, TEXT("MoveStatus Idle"));
						}
					}

				}
			}
			
				
			
			
		}
	
	
}
void UInvadedMovementComp::TurnServer_Implementation(float Rotation)
{
	TurnMulticast(Rotation);



}
bool UInvadedMovementComp::TurnServer_Validate(float Rotation)
{
	return true;
}
void UInvadedMovementComp::TurnMulticast_Implementation(float Rotation)
{
	AInvadedCharacter* PlayerRef = Cast<AInvadedCharacter>(GetOwner());

	if (PlayerRef)
	{
		AnimInstance = Cast<UInvadedAnimInstance>(PlayerRef->GetAnimInstance());
		
		if (AnimInstance)
		{
			
			//SetMoveStatus(EMoveStatus::MS_Rotating);
			if (Rotation == 90.0f)
			{
				AnimInstance->SetTurnMode(ETurnMode::TM_RightTurn);
			}
			else if (Rotation == -90.0f)
			{
				AnimInstance->SetTurnMode(ETurnMode::TM_LeftTurn);

			}
			else if (Rotation == 180.0f)
			{
				AnimInstance->SetTurnMode(ETurnMode::TM_BackwardTurn);

			}
			else
			{
				AnimInstance->SetTurnMode(ETurnMode::TM_None);
			}
		}
	}
	
}
bool UInvadedMovementComp::TurnMulticast_Validate(float Rotation)
{
	return true;
}
void UInvadedMovementComp::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(UInvadedMovementComp,bIsMoving);
	DOREPLIFETIME(UInvadedMovementComp,CurrentDestination);

}