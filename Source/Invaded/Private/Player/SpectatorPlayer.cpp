// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectatorPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "UI/InvadedStateMachine.h"
#include "GameModes/InvadedData.h"
#include "UI/MainGameWidget.h"

// Sets default values
ASpectatorPlayer::ASpectatorPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	RootComponent = CameraBoom;
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	
										  // Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	TopDownCameraComponent->bIsActive = true;

	if(!InvadedPS)
	InvadedPS = CreateDefaultSubobject<UInvadedData>(TEXT("InvadedPS"));
	

	CameraBoom->TargetArmLength = NewDesiredArmLength;
}
void ASpectatorPlayer::ToggleHUD()
{
	BP_ToggleHUD();
}
void ASpectatorPlayer::SetInvadedPS(UInvadedData * Value)
{
	InvadedPS = Value;
}
void ASpectatorPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsLocallyControlled())
	{
		if (UIStateMachineClass)
		{
			UIStateMachine = CreateWidget<UInvadedStateMachine>(GetWorld(), UIStateMachineClass);
			if (UIStateMachine)
			{
				UIStateMachine->AddToViewport();
			}

		}
	}
	else
	{
		ClientPossessedBy(NewController);
	}
}
void ASpectatorPlayer::ClientPossessedBy_Implementation(AController* NewController)
{
	if (UIStateMachineClass)
	{
		UIStateMachine = CreateWidget<UInvadedStateMachine>(GetWorld(), UIStateMachineClass);
		if (UIStateMachine)
		{
			UIStateMachine->AddToViewport();
		}
	}
}
bool ASpectatorPlayer::ClientPossessedBy_Validate(AController* NewController)
{
	return true;
}
// Called when the game starts or when spawned
void ASpectatorPlayer::BeginPlay()
{
	Super::BeginPlay();


	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CameraStart"), Cameras);
	if (Cameras.Num())
	{
		TopDownCameraComponent->SetWorldLocation(Cameras[0]->GetActorLocation());
	}
	
	
}

void ASpectatorPlayer::Zoom_Implementation(const float & Amount)
{
	NewDesiredArmLength = FMath::Clamp(NewDesiredArmLength + Amount, MinZoom, MaxZoom);

	while (!(FMath::IsNearlyEqual(CameraBoom->TargetArmLength, NewDesiredArmLength, 0.1f)))
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, NewDesiredArmLength, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.0f);
	}
}

void ASpectatorPlayer::MoveForward_Implementation(const float & Amount)
{
	FVector Location;
	FVector Rotation = GetTopDownCameraComponent()->GetComponentRotation().Vector();
	FVector Input = Rotation;

	Location = Amount * 15.0f*Input + GetTopDownCameraComponent()->GetComponentLocation();
	Location.Z = GetTopDownCameraComponent()->GetComponentLocation().Z;
	GetTopDownCameraComponent()->SetWorldLocation(Location);
}

void ASpectatorPlayer::MoveRight_Implementation(const float & Amount)
{
	FVector Location;
	FVector Rotation = GetTopDownCameraComponent()->GetComponentRotation().Vector();
	FVector Input = FRotationMatrix(GetTopDownCameraComponent()->GetComponentRotation()).GetScaledAxis(EAxis::Y);

	Location = Amount * 15.0f*Input + GetTopDownCameraComponent()->GetComponentLocation();
	Location.Z = GetTopDownCameraComponent()->GetComponentLocation().Z;

	GetTopDownCameraComponent()->SetWorldLocation(Location);
}

void ASpectatorPlayer::Rotate_Implementation(const float & Amount)
{
	APlayerController* PlayerCon = Cast<APlayerController>(GetController());
	if (PlayerCon)
	{
		float Roll = GetControlRotation().Roll;
		float Yaw, Pitch;
		PlayerCon->GetInputMouseDelta(Yaw, Pitch);
		Yaw = Amount * Yaw + GetControlRotation().Yaw;
		Pitch = Amount * Pitch + GetControlRotation().Pitch;
		PlayerCon->SetControlRotation(FRotator(Pitch, Yaw, Roll));

		GetTopDownCameraComponent()->SetWorldRotation(FRotator(Pitch, Yaw, Roll));
	}
}

// Called every frame
void ASpectatorPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpectatorPlayer::ToggleChatWindow()
{
	if (UIStateMachine)
	{
		UIStateMachine->ToggleState(TEXT("UIChatWindow"));
	}
}

void ASpectatorPlayer::TogglePauseUI()
{
	
	if (UIPause)
	{
		UIPause->RemoveFromParent();
		UIPause = nullptr;
		if (UIStateMachine)
		{
			UIStateMachine->SetVisibility(ESlateVisibility::Visible);
		}
		InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

	}
	else if (UIPauseClass)
	{
		UIPause = CreateWidget<UInvadedStateMachine>(GetWorld(), UIPauseClass);
		if (UIStateMachine)
		{
			UIStateMachine->SetVisibility(ESlateVisibility::Collapsed);
		}
		UIPause->AddToViewport();
		InvadedPS->SetPlayerMode(EPlayerMode::PM_UI);
	}
}

void ASpectatorPlayer::GameWon()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (UIStateMachine)
		{
			UIStateMachine->PushState(TEXT("ScoreboardUI"));

			if (UIStateMachine->FindStateByName(TEXT("GameUI")))
			{
				UIStateMachine->FindStateByName(TEXT("GameUI"))->SetVisibility(ESlateVisibility::Collapsed);
				BP_CollapseHUD();
			}
		}
	}
	else
	{
		GameWonClient();
	}
	
}

void ASpectatorPlayer::GameWonClient_Implementation()
{
	GameWon();
}
bool ASpectatorPlayer::GameWonClient_Validate()
{
	return true;
}
void ASpectatorPlayer::ToggleDicePod()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (UIStateMachine)
		{
			if (MainGameWidget)
			{
				MainGameWidget->ToggleDicePod();
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Invaded State Machine"));
		}
	}
	else
	{
		ToggleDicePodClient();
	}
}
void ASpectatorPlayer::ToggleDicePodClient_Implementation()
{
	ToggleDicePod();
}
bool ASpectatorPlayer::ToggleDicePodClient_Validate()
{
	return true;
}


