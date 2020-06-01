// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedPCManager.h"
#include "Camera/CameraComponent.h"
#include "InvadedPlayer.h"
#include "InvadedCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "InvadedGM.h"
#include "FightGame.h"
#include "Kismet/GameplayStatics.h"

AInvadedPCManager::AInvadedPCManager()
{
	PrimaryActorTick.bCanEverTick = true;
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));

	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	YOffset = 100.0f;

	bReplicates = true;
}
void AInvadedPCManager::BeginPlay()
{
	Super::BeginPlay();

	if (fCurve)
	{
		MyTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));
		MyTimeline->SetTimelineFinishedFunc(TimelineFinished);

		MyTimeline->SetLooping(false);
		MyTimeline->SetIgnoreTimeDilation(true);

	}
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(GetWorld()->GetGameState());
	if (InvadedGS)
	{
		YOffset = YOffset * InvadedGS->GetMapScale();
	}
}

void AInvadedPCManager::PlayFightAnim(UCameraComponent* Camera,AInvadedPlayer* PlayerValue,AInvadedCharacter* EnemyValue)
{
	CurrentCamera = Camera;
	StartLocation = CurrentCamera->GetComponentLocation();
	StartRotation = GetOwningPlayerController()->GetControlRotation();

	this->Player = PlayerValue;

	if (FMath::IsNearlyEqual(Player->GetActorLocation().X, EnemyValue->GetActorLocation().X, 20.0f))
	{
		EndLocation = FVector(Player->GetActorLocation().X + YOffset, (Player->GetActorLocation().Y + EnemyValue->GetActorLocation().Y) / 2, Player->GetActorLocation().Z);
		EndRotation = UKismetMathLibrary::FindLookAtRotation(EndLocation, FVector(Player->GetActorLocation().X, (Player->GetActorLocation().Y + EnemyValue->GetActorLocation().Y) / 2, Player->GetActorLocation().Z));
	}
	else
	{
		EndLocation = FVector((Player->GetActorLocation().X + EnemyValue->GetActorLocation().X) / 2, Player->GetActorLocation().Y + YOffset, Player->GetActorLocation().Z);
		EndRotation = UKismetMathLibrary::FindLookAtRotation(EndLocation, FVector((Player->GetActorLocation().X + EnemyValue->GetActorLocation().X) / 2, Player->GetActorLocation().Y, Player->GetActorLocation().Z));
		
	}
	
	bReverse = false;
	

	MyTimeline->Play();
}
void AInvadedPCManager::ReturnToStartLocation(UCameraComponent * Camera)
{
	EndLocation = StartLocation;
	EndRotation = StartRotation;
	StartLocation = CurrentCamera->GetComponentLocation();
	StartRotation = GetOwningPlayerController()->GetControlRotation();

	MyTimeline->Play();
}
void AInvadedPCManager::PlayReverseAnim()
{
	bReverse = true;
	MyTimeline->Reverse();
}

void AInvadedPCManager::TimelineFloatReturn(float Value)
{
	if (CurrentCamera)
	{
		CurrentCamera->SetWorldLocation(FMath::Lerp(StartLocation, EndLocation, Value));
		CurrentCamera->SetWorldRotation(FMath::Lerp(StartRotation, EndRotation, Value));
		GetOwningPlayerController()->SetControlRotation(FMath::Lerp(StartRotation, EndRotation, Value));
	}
	
	
}

void AInvadedPCManager::OnTimelineFinished()
{
	if (Player)
	{
		if (bReverse)
		{
			Player->UnlockCamera();

			if(Player->GetFightGameComponent())
			Player->GetFightGameComponent()->EndGameReached();

		
		}
		else
		{
			Player->LockCamera();
			Player->InitFightGame();
		
		}
		
	}
}
