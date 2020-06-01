// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedPC.h"
#include "AI/NavigationSystemBase.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "InvadedPlayer.h"
#include "InvadedAnimInstance.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "CellBase.h"
#include "CellCorner.h"
#include "UnrealNetwork.h"
#include "InvadedData.h"
#include "InvadedGS.h"
#include "InvadedGM.h"
#include "Kismet/GameplayStatics.h"
#include "InvadedMovementComp.h"
#include "Player/SpectatorPlayer.h"
#include "Interfaces/CameraInterface.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"




AInvadedPC::AInvadedPC()
{
	bRotateCamera = false;
	bReplicates = true;
	bReplicateMovement =false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bShowMouseCursor = true;
	bAutoManageActiveCameraTarget = false;
}
void AInvadedPC::Spectate()
{
	
	FActorSpawnParameters SpawnParams;
	FTransform Transform = PlayerRef->GetActorTransform();
	UnPossess();

	SpectatorPlayerRef = Cast<ASpectatorPlayer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, SpectatorPlayerClass, Transform));
	if (SpectatorPlayerRef != nullptr)
	{
		UGameplayStatics::FinishSpawningActor(SpectatorPlayerRef, Transform);
		//SpectatorPlayerRef->SetInvadedPS(PlayerRef->GetInvadedPS());
		SpectatorPlayerRef->GetInvadedPS()->SetPlayerColor(PlayerRef->GetInvadedPS()->GetPlayerColor());
		SpectatorPlayerRef->GetInvadedPS()->SetFlagColor(PlayerRef->GetInvadedPS()->GetFlagColor());
		SpectatorPlayerRef->GetInvadedPS()->SetPlayerName(PlayerRef->GetInvadedPS()->GetPlayerName());
		SpectatorPlayerRef->GetInvadedPS()->SetTurnScore(PlayerRef->GetInvadedPS()->GetTurnScore());
		PlayerRef->Destroy();
	
		if (GetWorld()->GetGameState<AInvadedGS>())
		{
			GetWorld()->GetGameState<AInvadedGS>()->AddInvadedPlayerState(SpectatorPlayerRef->GetInvadedPS());
		}

		PlayerRef = nullptr;
		Possess(SpectatorPlayerRef);
		if (SpectatorPlayerRef)
			SetViewTarget(SpectatorPlayerRef);
	}
	
	
}
void AInvadedPC::LockButtons()
{
	bLockButtons = true;
}
void AInvadedPC::UnlockButtons()
{
	bLockButtons = false;
}
void AInvadedPC::AcknowledgePossession(APawn * P)
{
	Super::AcknowledgePossession(P);
}
void AInvadedPC::LookAtScreen()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(),FName("LoadingScreenCamera"),Actors);
	for (AActor* Actor : Actors)
	{
		SetViewTargetWithBlend(Actor);
		break;
	}
}
void AInvadedPC::LookAtPawn_Implementation()
{
	PlayerRef = Cast<AInvadedPlayer>(GetPawn());
	if (PlayerRef)
	{
		SetViewTargetWithBlend(PlayerRef);
	}
}
bool AInvadedPC::LookAtPawn_Validate()
{
	return true;
}
 void AInvadedPC::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
	PlayerRef = Cast<AInvadedPlayer>(GetPawn());
	if (PlayerRef)
	{
		InvadedPS = PlayerRef->GetInvadedPS();
	}	
}
void AInvadedPC::BeginPlayingState()
{
	Super::BeginPlayingState();
	PlayerRef = Cast<AInvadedPlayer>(GetPawn());


}
void AInvadedPC::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (bRotateCamera)
	{
		RotateCamera();
	}
}

void AInvadedPC::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	
	InputComponent->BindAction("OnUse", IE_Pressed, this, &AInvadedPC::OnUse).bConsumeInput=false;
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AInvadedPC::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AInvadedPC::ZoomOut);
	InputComponent->BindAction("RotateCamera", IE_Pressed, this, &AInvadedPC::StartRotating);
	InputComponent->BindAction("RotateCamera", IE_Released, this, &AInvadedPC::StopRotating);
	InputComponent->BindAction("Drop", IE_Released, this, &AInvadedPC::Drop);
	InputComponent->BindAction("Pause", IE_Pressed, this, &AInvadedPC::TogglePauseUI);
	InputComponent->BindAction("ChatWindow", IE_Pressed, this, &AInvadedPC::ToggleChatWindow);

	InputComponent->BindAxis("MoveForward", this, &AInvadedPC::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AInvadedPC::MoveRight);

	
}
void AInvadedPC::MoveForward(float Value)
{
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(GetWorld()->GetGameState());
	if (InvadedGS)
	{
		ICameraInterface * CameraInterface = Cast<ICameraInterface>(GetPawn());
		if (CameraInterface)
			CameraInterface->Execute_MoveForward(GetPawn(), InvadedGS->GetMapInfo().MapScale*Value);
	}
	

}
void AInvadedPC::Drop_Implementation()
{
	if (!PlayerRef)
	{
		PlayerRef = Cast<AInvadedPlayer>(GetPawn());
	}
	if (PlayerRef&& InvadedPS->GetPlayerMode() == EPlayerMode::PM_Base)
	{
		if (PlayerRef->HasFlag())
		{
			PlayerRef->DropFlag();
		}
	}
}
bool AInvadedPC::Drop_Validate()
{
	return true;
}
void AInvadedPC::MoveRight(float Value)
{
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(GetWorld()->GetGameState());
	if (InvadedGS)
	{
		ICameraInterface * CameraInterface = Cast<ICameraInterface>(GetPawn());
		if (CameraInterface)
			CameraInterface->Execute_MoveRight(GetPawn(), InvadedGS->GetMapInfo().MapScale*Value);
	}
	
	
}
void AInvadedPC::OnUseServer_Implementation()
{
	OnUse();
}
bool AInvadedPC::OnUseServer_Validate()
{
	return true;
}
void AInvadedPC::OnUse()
{
		if (!PlayerRef)
		{
		PlayerRef = Cast<AInvadedPlayer>(GetPawn());
		}
		if (!InvadedPS)
		{
			if(PlayerRef)
			InvadedPS = PlayerRef->GetInvadedPS();
		}
		if (InvadedPS)
		{
			if (InvadedPS->GetMoveStatus() == EMoveStatus::MS_Idle)
			{
				if (InvadedPS->GetPlayerMode() != EPlayerMode::PM_UI)
				{


					if (Role < ROLE_Authority)
					{
						OnUseServer();
					}
					else
					{
						PlayerRef = Cast<AInvadedPlayer>(GetPawn());
						if (PlayerRef)
							InvadedPS = PlayerRef->GetInvadedPS();

						if (PlayerRef&&InvadedPS)
						{

							if (InvadedPS->GetPlayerMode() == EPlayerMode::PM_Base)
							{

								if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_Move)
								{

									if (PlayerRef->GetLastFocusedPlayer())
									{
										if (PlayerRef->GetLastUsedCard())
										{
											PlayerRef->GetLastUsedCard()->SpawnEffect(PlayerRef, PlayerRef->GetLastFocusedPlayer());
										}
										else
										{
											InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);
										}
										
									}

								}
								else
								{
									if (PlayerRef->GetOversteppedCell() == PlayerRef->GetLastFocusedCell())
										PlayerRef->SetOversteppedCell(PlayerRef->GetOversteppedCell());
									UE_LOG(LogTemp, Warning, TEXT("Player connected"));
									if (InvadedPS->GetIsCurrentPlayer() && InvadedPS->GetRemainingMoves() > 0)
										PlayerRef->RetraceThePath();
								}

							}
							else if (InvadedPS->GetPlayerMode() != EPlayerMode::PM_Fight)
							{
								if (InvadedPS->GetPlayerMode() == EPlayerMode::PM_CornerCellSelection)
								{
									if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_MoveToCornerCell)
									{
										
										if (PlayerRef->GetLastFocusedCell() && PlayerRef->GetLastFocusedPlayer())
										{
											if (PlayerRef->GetLastFocusedCell()->IsA<ACellCorner>())
											{
												
												if (PlayerRef->GetLastUsedCard())
												{
													PlayerRef->GetLastUsedCard()->SpawnEffect(PlayerRef, PlayerRef->GetLastFocusedPlayer());
												}
												else
												{
													InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
													InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);
												}	
											}
										}

									}
									else if (InvadedPS->GetEnemyAction() != EEnemyAction::EA_Base)
									{
										InvadedPS->SetPlayerMode(EPlayerMode::PM_PlayerSelection);
									}
									else if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_Move)
									{
										if (PlayerRef->GetLastFocusedPlayer() && PlayerRef->GetLastFocusedCell())
										{
											if (PlayerRef->GetLastUsedCard())
											{
												PlayerRef->GetLastUsedCard()->SpawnEffect(PlayerRef, PlayerRef->GetLastFocusedPlayer());
											}
											else
											{
												InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
												InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);
											}

										}

									}
									else
									{
										if (PlayerRef->GetLastFocusedCell())
										{
											if (PlayerRef->GetLastFocusedCell()->IsA<ACellCorner>())
											{
												if (PlayerRef->GetLastUsedCard())
												{
													PlayerRef->GetLastUsedCard()->SpawnEffect(PlayerRef, PlayerRef->GetLastFocusedPlayer());
												}
												else
												{
													InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
													InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);
												}
											}
										}

									}

								}
								else if (InvadedPS->GetPlayerMode() == EPlayerMode::PM_PlayerSelection)
								{

									if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_LoseBattle)
									{
										if (PlayerRef->GetLastFocusedPlayer())
										{
											if (PlayerRef->GetLastUsedCard())
											{
												PlayerRef->GetLastUsedCard()->SpawnEffect(PlayerRef, PlayerRef->GetLastFocusedPlayer());
											}
											else
											{
												InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
												InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);
											}
											
										}
									}
									else if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_MoveToCornerCell)
									{
										PlayerRef->ClearSelections();
										InvadedPS->SetPlayerMode(EPlayerMode::PM_CornerCellSelection);
									}
									else if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_LoseTurn)
									{
										if (PlayerRef->GetLastFocusedPlayer())
										{
											if (PlayerRef->GetLastUsedCard())
											{
												PlayerRef->GetLastUsedCard()->SpawnEffect(PlayerRef, PlayerRef->GetLastFocusedPlayer());
											}
											else
											{
												InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
												InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);
											}
										}
									}
									else if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_Move)
									{
										if (PlayerRef->GetLastFocusedPlayer())
										{											
											InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
										}
									}


								}
							}



						}
					}
				}
			}
			
		}
		
}
class UInvadedData* AInvadedPC::GetCurrentGSPlayer() const
{
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGS)
	{
		return InvadedGS->GetCurrentPlayer();
	}
	return nullptr;
}
int32 AInvadedPC::GetCurrentTurn() const
{
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGS)
	{
		return InvadedGS->GetCurrentTurn();
	}
	return 0;
}
void AInvadedPC::AddLoadingWidget()
{
	if (LoadingWidgetClass)
	{
		LoadingWidget = CreateWidget<UUserWidget>(GetWorld(),LoadingWidgetClass);
		if (LoadingWidget)
			LoadingWidget->AddToViewport();
	}
}
void AInvadedPC::RemoveLoadingWidget()
{
	if (LoadingWidget)
		LoadingWidget->RemoveFromParent();
}
void AInvadedPC::ZoomIn()
{
	ICameraInterface * CameraInterface = Cast<ICameraInterface>(GetPawn());
	if (CameraInterface)
		CameraInterface->Execute_Zoom(GetPawn(), ZoomAmount);
}

void AInvadedPC::ZoomOut()
{
	ICameraInterface * CameraInterface = Cast<ICameraInterface>(GetPawn());
	if (CameraInterface)
		CameraInterface->Execute_Zoom(GetPawn(), -ZoomAmount);
}
void AInvadedPC::SetMoveDestination(FVector Destination)
{
	SetMoveDestinationClient(Destination);	
}
void AInvadedPC::SetMoveDestinationClient_Implementation(FVector Destination)
{
	PlayerRef = Cast<AInvadedPlayer>(GetPawn());
	if (PlayerRef)
	{
		PlayerRef->GetInvadedMovementComp()->SetDestination(Destination);
	}
}
bool AInvadedPC::SetMoveDestinationClient_Validate(FVector Destination)
{
	return true;
}

void AInvadedPC::SetNewZRotation(float ZRotation)
{
	if (ZRotation != 0.0f)
	{
		PlayerRef = Cast<AInvadedPlayer>(GetPawn());
		if (PlayerRef)
			PlayerRef->GetInvadedMovementComp()->Turn(ZRotation);
	}	
}
void AInvadedPC::RotateCamera()
{
	ICameraInterface * CameraInterface = Cast<ICameraInterface>(GetPawn());
	if (CameraInterface)
		CameraInterface->Execute_Rotate(GetPawn(), 4.0f);
}

void AInvadedPC::StartRotating()
{
	bRotateCamera = true;
}

void AInvadedPC::StopRotating()
{
	bRotateCamera = false;
}
void AInvadedPC::ThrowDice()
{
	if (!InvadedPS)
	{
		InvadedPS = PlayerRef->GetInvadedPS();
	}
	if (InvadedPS)
	{
		if (InvadedPS->GetPlayerMode() != EPlayerMode::PM_UI)
		{
			if (Role < ROLE_Authority)
			{
				ThrowDiceServer();
			}
			else
			{
				AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
				if (InvadedGM)
				{
					InvadedGM->ThrowDice();
				}
			}
		}
	}

}
void AInvadedPC::TogglePauseUI()
{
	if (!bLockButtons)
	{
		if (!PlayerRef)
		{
			PlayerRef = Cast<AInvadedPlayer>(GetPawn());
		}
		if (SpectatorPlayerRef)
		{
			SpectatorPlayerRef->TogglePauseUI();
		}
		if (PlayerRef)
		{
			PlayerRef->TogglePauseUI();
		}
	}
	
}
void AInvadedPC::ToggleChatWindow()
{
	if (!bLockButtons)
	{
		if (!PlayerRef)
		{
			PlayerRef = Cast<AInvadedPlayer>(GetPawn());
		}
		if (SpectatorPlayerRef)
		{
			SpectatorPlayerRef->ToggleChatWindow();
		}
		if (PlayerRef)
		{
			PlayerRef->ToggleChatWindow();
		}
	}
	
}
void AInvadedPC::ThrowDiceServer_Implementation()
{
	ThrowDice();
}
bool AInvadedPC::ThrowDiceServer_Validate()
{
	return true;
}
void AInvadedPC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInvadedPC,SpectatorPlayerRef);
	DOREPLIFETIME(AInvadedPC,PlayerRef);
}