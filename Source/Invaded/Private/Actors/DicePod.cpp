// Fill out your copyright notice in the Description page of Project Settings.

#include "DicePod.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Dice.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/InvadedGM.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "AI/InvadedAIController.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ADicePod::ADicePod()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SM_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("S_Mesh"));
	RootComponent = SM_Mesh;

	Dummy = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy"));
	Dummy->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Dummy);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(Camera);


	DiceValue = 0;

	CurveDelay = 5.0f;

	AlphaValue = 0.0f;

	RotationYaw = 0.0f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ADicePod::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = Dummy->GetComponentLocation();
	StartRotation = Dummy->GetComponentRotation();

	NewQuat = StartRotation.Quaternion();

	if (Material)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(Material, this);

		SM_Mesh->SetMaterial(0, DynMaterial);
	}
	
}

void ADicePod::ToggleDicePod()
{
	if (Role < ROLE_Authority)
	{
		ToggleDicePodServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			InvadedGM->ToggleDicePod();
		}

	}
}

void ADicePod::EndDice(bool RepeatThrow)
{
	if (Role < ROLE_Authority)
	{
		EndDiceServer(RepeatThrow);
	}
	else
	{
		AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
		
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (CurrentOwner&&InvadedGS->GetCurrentPlayer()==CurrentOwner)
		{
			UE_LOG(LogTemp, Warning, TEXT("EndDice %s"), *InvadedGS->GetCurrentPlayer()->GetPlayerName());
			if (SpecifiedPlayer)
			{
				InvadedGS->GetCurrentPlayer()->AddRemainingMoves(DiceValue);
				DiceValue = 0;
				SpecifiedPlayer = nullptr;
				InvadedGM->FirstFight();
				
			}
			else
			{
				
				if (InvadedGM&&InvadedGS->GetCurrentPlayer()->GetIsAIPlayer() && RepeatThrow)
				{
					InvadedGS->GetCurrentPlayer()->SetAdditionalThrow(false);
					InvadedGM->ThrowDice();


				}
				else
				{
				
					InvadedGS->GetCurrentPlayer()->SetHasTrownDice(true);
					InvadedGS->GetCurrentPlayer()->AddRemainingMoves(DiceValue);
					//InvadedGS->GetCurrentPlayer()->AddRemainingMoves(1);
					DiceValue = 0;

				//	InvadedGS->GetCurrentPlayer()->SetAdditionalThrow(false);
				}

				if (!RepeatThrow)
				{
					InvadedGS->GetCurrentPlayer()->SetHasTrownDice(true);
				//	InvadedGS->GetCurrentPlayer()->SetAdditionalThrow(false);
				}
			}
			
		}
		
		

	}
}

void ADicePod::UpdateMaterial_Implementation()
{
	if (DynMaterial)
	{
		DynMaterial->SetVectorParameterValue("P_Color1", FirstColor);

		DynMaterial->SetVectorParameterValue("P_Color2", SecondColor);	
	}
}
bool ADicePod::UpdateMaterial_Validate()
{
	return true;
}

void ADicePod::EndDiceServer_Implementation(bool RepeatThrow)
{
	EndDice(RepeatThrow);
}
bool ADicePod::EndDiceServer_Validate(bool RepeatThrow)
{
	return true;
}

void ADicePod::ToggleDicePodServer_Implementation()
{
	ToggleDicePod();
}
bool ADicePod::ToggleDicePodServer_Validate()
{
	return true;
}
// Called every frame
void ADicePod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bProjectCamera || bRotateCamera)
	{
		ProjectCameraCurve();
	}
	

	if (Dummy&&Camera&&SceneCapture)
	{
		Camera->SetWorldLocation(Dummy->GetComponentLocation());
		Camera->SetWorldRotation(NewQuat);

		SceneCapture->SetWorldLocation(Dummy->GetComponentLocation());
		SceneCapture->SetWorldRotation(NewQuat);

	//	UE_LOG(LogTemp, Warning, TEXT("SC %s"), *SceneCapture->GetComponentRotation().ToString());
	}

	if (Material)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(Material, this);

		DynMaterial->SetVectorParameterValue("P_Color1", FirstColor);
		DynMaterial->SetVectorParameterValue("P_Color2", SecondColor);
		SM_Mesh->SetMaterial(0, DynMaterial);
	}
	
}
void ADicePod::ProjectCameraCurve()
{

	if (!(Role < ROLE_Authority))
	{
		if (Dice&&Camera)
		{
			FVector Location = FVector(Dice->GetActorLocation().X, Dice->GetActorLocation().Y, Dice->GetActorLocation().Z + DistanceFromDice);
			
			
			if (bRotateCamera)
			{
				
				FRotator Rotator = Dice->Camera->GetRelativeTransform().GetRotation().Rotator()+ FRotator(0.0f,Dice->GetActorRotation().Yaw, Dice->GetActorRotation().Roll);
		
				AlphaValue = FMath::FInterpConstantTo(AlphaValue, 1.0f, GetWorld()->GetDeltaSeconds(), RotationSpeed);

			
				NewQuat = FQuat::Slerp(FRotator(270.0f, 0.0f, 0.0f).Quaternion(), Rotator.Quaternion(), AlphaValue);
				Dummy->SetWorldRotation(FQuat::Slerp(FRotator(270.0f,0.0f,0.0f).Quaternion(), Rotator.Quaternion(),AlphaValue));
				Dummy->SetWorldRotation(Rotator);
			

				if (FMath::IsNearlyEqual(AlphaValue, 1.0f,0.05f))
				{
					NewQuat = Rotator.Quaternion();
					Dummy->SetWorldRotation(Rotator);
					AlphaValue = 0.0f;
					bRotateCamera = false;
				}
			}
			
			if (bProjectCamera)
			{
				Dummy->SetWorldLocation(FMath::VInterpConstantTo(Dummy->GetComponentLocation(), Location, GetWorld()->GetDeltaSeconds(), LocationSpeed));

				if (FMath::IsNearlyEqual(Dummy->GetComponentLocation().Z, Location.Z, 10.0f))
				{
				
					Dummy->SetWorldLocation(Location);
					bProjectCamera = false;


				}
			}
			if (!bProjectCamera && !bRotateCamera)
			{
				GetWorld()->GetTimerManager().SetTimer(CameraHandle, this, &ADicePod::EndCameraCurve,CurveDelay, false);
			}


		}
	}
	
}
void ADicePod::EndCameraCurve()
{
	
	Dice->Destroy();
	
	bool bTemp = bAdditionalThrow;
	if (!bAdditionalThrow)
	{

		if(!SpecifiedPlayer)
		ToggleDicePod();
		
	}
	else
	{
		bAdditionalThrow = false;
	}

	NewQuat = StartRotation.Quaternion();
	Dummy->SetWorldLocation(StartLocation);
	Dummy->SetWorldRotation(StartRotation);

	EndDice(bTemp);
		
	
}
void ADicePod::SpawnDice(const bool AdditionalThrow)
{
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGS)
	{
		InvadedGS->GetCurrentPlayer()->SetLastRemainingMoves(InvadedGS->GetCurrentPlayer()->GetRemainingMoves());
		SetFirstColor(InvadedGS->GetCurrentPlayer()->GetFlagColor());
		SetSecondColor(InvadedGS->GetCurrentPlayer()->GetPlayerColor());


		bRotateCamera = false;
		bProjectCamera = false;
		Dummy->SetWorldLocation(StartLocation);
		FActorSpawnParameters SpawnParams;
		float Pitch = FMath::FRandRange(-180.0f, 180.0f) * 90.f;
		float Yaw = FMath::FRandRange(-180.0f, 180.0f)*90.f;
		float Roll = FMath::FRandRange(-180.0f, 180.0f) * 90.f;

		CurrentOwner = InvadedGS->GetCurrentPlayer();

		Dice = GetWorld()->SpawnActorDeferred<ADice>(DiceClass, FTransform(FRotator(Pitch, Yaw, Roll), GetActorLocation() + FVector(0.0f, 0.0f, DiceThrowHeight)));
		if (Dice)
		{
			Dice->GetMesh()->AddForce(Force);
			Dice->SetFirstColor(InvadedGS->GetCurrentPlayer()->GetFlagColor());

			UE_LOG(LogTemp, Warning, TEXT("Flag Color %s"), *InvadedGS->GetCurrentPlayer()->GetFlagColor().ToString());
			Dice->SetSecondColor(InvadedGS->GetCurrentPlayer()->GetPlayerColor());

			UGameplayStatics::FinishSpawningActor(Dice, FTransform(FRotator(Pitch, Yaw, Roll), GetActorLocation() + FVector(0.0f, 0.0f, DiceThrowHeight)));
		}

		bAdditionalThrow = AdditionalThrow;

		if (!bAdditionalThrow)
			InvadedGS->GetCurrentPlayer()->SetAdditionalThrow(false);

		
	
		//InvadedGS->GetCurrentPlayer()->SetHasTrownDice(true);
	}

}
void ADicePod::SpawnDiceSpecified(UInvadedData* SpecifiedPlayerValue)
{
	this->SpecifiedPlayer = SpecifiedPlayerValue;

	SpawnDice(false);
}
void ADicePod::Toggle()
{
	if (Dice)
	{
		bRotateCamera = true;
		bProjectCamera = true;
	}

}

void ADicePod::SetDiceValue(uint8 Value)
{
	if (Role < ROLE_Authority)
	{
		SetDiceValueServer(Value);
	}
	else
	{
		DiceValue += Value;
	}
	
}

void ADicePod::SetFirstColor(FLinearColor Value)
{
	FirstColor = Value;	

	UpdateMaterial();
}

void ADicePod::SetSecondColor(FLinearColor Value)
{
	SecondColor = Value;	

	UpdateMaterial();
}

void ADicePod::SetDiceValueServer_Implementation(uint8 Value)
{
	SetDiceValue(Value);
}
bool ADicePod::SetDiceValueServer_Validate(uint8 Value)
{
	return true;
}
void ADicePod::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADicePod, CurrentOwner);
	DOREPLIFETIME(ADicePod, DiceValue);
	DOREPLIFETIME(ADicePod, FirstColor);
	DOREPLIFETIME(ADicePod, SecondColor);
	DOREPLIFETIME(ADicePod, NewQuat);
}