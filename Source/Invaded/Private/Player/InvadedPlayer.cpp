// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedPlayer.h"
#include "UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "InvadedMovementComp.h"
#include "Materials/Material.h"
#include "CellBase.h"
#include "InvadedPC.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "AStar.h"
#include "InvadedGM.h"
#include "Blueprint/UserWidget.h"
#include "InvadedAnimInstance.h"
#include "InvadedData.h"
#include "InvadedGS.h"
#include "GridGeneratorAlg.h"
#include "Algo/Reverse.h"
#include "InvadedPCManager.h"
#include "CardBase.h"
#include "CellCorner.h"
#include "TimerManager.h"
#include "UI/InvadedStateMachine.h"
#include "GameFramework/PlayerState.h"
#include "UI/MainGameWidget.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "PlayerSettings.h"
#include "Async.h"


void AInvadedPlayer::ThrowDiceSpecified(UInvadedData * Value)
{
	if (Role < ROLE_Authority)
	{
		ThrowDiceSpecifiedServer(Value);
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			InvadedGM->ThrowDiceSpecified(Value);
		}
	}
}
void AInvadedPlayer::ThrowDiceSpecifiedServer_Implementation(UInvadedData * Value)
{
	ThrowDiceSpecified(Value);
}
bool AInvadedPlayer::ThrowDiceSpecifiedServer_Validate(UInvadedData * Value)
{
	return true;
}
void AInvadedPlayer::ToggleHUD()
{
	BP_ToggleHUD();
}
AInvadedPlayer::AInvadedPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UInvadedMovementComp>(ACharacter::CharacterMovementComponentName))
{

	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

										  // Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

															 


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	CameraBoom->TargetArmLength = NewDesiredArmLength;
	bReplicates = true;

	MinZoom = 20.0f;
	MaxZoom = 600.0f;

	bFightCleaned = false;
}
void AInvadedPlayer::InitializePlayer()
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		InitializePlayerServer();
	}	
	else if(Role == ROLE_Authority&&!bInitialized)
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
		if (InvadedGS&&InvadedGM)
		{
			bInitialized = true;
			//InvadedGS->AddInvadedPlayerState(GetInvadedPS());
			InvadedGS->SetCurrentNumOfPlayers(InvadedGS->GetCurrentNumOfPlayers() + 1);
			UE_LOG(LogTemp, Warning, TEXT("Initializing %d %d"), InvadedGS->GetCurrentNumOfPlayers(), InvadedGS->GetNumberOfPlayers());
			if (InvadedGS->GetCurrentNumOfPlayers() >= InvadedGS->GetNumberOfPlayers())
			{

				InvadedGM->InitPlayerLocation();
			}
		}
	}
	if (Role == ROLE_Authority)
	{
		UpdateWeaponMeshMulticast(GetInvadedPS()->GetWeaponIndex());

		UpdatePlayerMeshMulticast(GetInvadedPS()->GetMeshIndex());

	}
}
void AInvadedPlayer::InitializePlayerServer_Implementation()
{
	InitializePlayer();
}
bool AInvadedPlayer::InitializePlayerServer_Validate()
{
	return true;
}
void AInvadedPlayer::UpdateWeaponMeshMulticast_Implementation(int32 Index)
{
	if (WeaponArray.Num() > Index)
	{
		SM_Rifle->SetStaticMesh(WeaponArray[Index]);
	}	
}
bool AInvadedPlayer::UpdateWeaponMeshMulticast_Validate(int32 Index)
{
	return true;
}
void AInvadedPlayer::UpdatePlayerMeshMulticast_Implementation(int32 Index)
{
		if (MeshArray.Num() > Index && AnimationBlueprintArray.Num() > Index)
		{
			GetMesh()->SetSkeletalMesh(MeshArray[Index]);
			GetMesh()->SetAnimInstanceClass(AnimationBlueprintArray[Index]);
			AnimInstance = Cast<UInvadedAnimInstance>(GetMesh()->GetAnimInstance());
		}	
}
bool AInvadedPlayer::UpdatePlayerMeshMulticast_Validate(int32 Index)
{
	return true;
}
void AInvadedPlayer::ToggleChatWindow()
{
	if (InvadedStateMachine)
	{
		InvadedStateMachine->ToggleState(TEXT("UIChatWindow"));
	}
}

void AInvadedPlayer::EnterFightMode(AActor* Enemy)
{
	Super::EnterFightMode(Enemy);
	//AddFightUI("UIFight");
	PlayFightAnim(Cast<AInvadedCharacter>(Enemy));
}

void AInvadedPlayer::BeginPlay()
{
	Super::BeginPlay();
	AInvadedPC* InvadedPC = Cast<AInvadedPC>(GetController());
	if (InvadedStateMachineClass)
	{
		InvadedStateMachine = CreateWidget<UInvadedStateMachine>(GetWorld(), InvadedStateMachineClass);
	}
	AddLoadingWidget();
	SetBoundaries();
	FAStar::Shutdown();
	
	
	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CameraStart"), Cameras);
	if (Cameras.Num())
	{
		MaxZoom = Cameras[0]->GetActorLocation().Z;
	}

	HighlightDelegate.BindUFunction(this, FName("FindPath"), OversteppedCell, LastFocusedCell);
	GetWorldTimerManager().SetTimer(HighlightTimerHandle,HighlightDelegate, HighlightFrequency, true);
	
	InvadedMovementComp = Cast<UInvadedMovementComp>(GetCharacterMovement());
	AnimInstance = Cast<UInvadedAnimInstance>(GetMesh()->GetAnimInstance());
}
void AInvadedPlayer::RemoveWidgets()
{
	if (Role < ROLE_Authority||IsLocallyControlled())
	{
		if (InvadedStateMachine)
		{
			InvadedStateMachine->RemoveFromParent();
		}
	}
	else
	{
		RemoveWidgetsClient();
	}
}
void AInvadedPlayer::CallTurnEnd()
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		CallTurnEndServer();
	}
	else
	{
		if (bThrownDice)
		{
			Super::CallTurnEnd();
			ClearSelections();
		}
		
	}
}
void AInvadedPlayer::RemoveWidgetsClient_Implementation()
{
	RemoveWidgets();
}
bool AInvadedPlayer::RemoveWidgetsClient_Validate()
{
	return true;
}
void AInvadedPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (FAStar::IsThreadFinished()&&!bPathFound&&IsLocallyControlled()&&InvadedMovementComp->IsMoving()==false)
	{
			
			
		ClearLastPath();
		Path = FAStar::GetPath();
			
		SetCurrentPath(FAStar::GetPath());
		FAStar::Shutdown();
			
			
				
		for (int8 i = 0; i <Path.Num(); ++i)
		{
					
			if (GetInvadedPS()->GetEnemyAction() == EEnemyAction::EA_Base)
			{
				if (GetInvadedPS()->GetRemainingMoves() - 1 < i)
					Path[i]->SetHighlightState(EHighlightState::HC_Negative);
			}
			else if (GetInvadedPS()->GetEnemyAction() == EEnemyAction::EA_Move&&LastFocusedPlayer)
			{
				if (i>=2)
					Path[i]->SetHighlightState(EHighlightState::HC_Negative);
			}

			Path[i]->OnBeginFocus();
		}
			
			
		bPathFound = true;
			
			
			
			
	}
	if (GetInvadedPS())
	{
		
		if (IsPlayerControlled() && GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_Base)
			HighlightCell();

		if (IsPlayerControlled() && GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_CornerCellSelection)
			HighlightCornerCell();

		if (IsPlayerControlled() && GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_PlayerSelection)
			HighlightPlayer();

		if (IsPlayerControlled() && GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_Base)
			FindOversteppedCell();
	}
		
}

void AInvadedPlayer::SetPlayerColor(const FLinearColor & Value)
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		SetPlayerColorServer(Value);
	}
	else
	{
		if (GetInvadedPS())
			GetInvadedPS()->SetPlayerColor(Value);
	}
}

void AInvadedPlayer::SetFightCleaned(bool Value)
{
	if (Role < ROLE_Authority)
	{
		SetFightCleanedServer(Value);
	}
	else
	{

	}
}



void AInvadedPlayer::AddLoadingWidget()
{
	if (IsLocallyControlled())
	{
		if (LoadingWidgetClass)
		{
			LoadingWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingWidgetClass);
			if (LoadingWidget)
				LoadingWidget->AddToViewport();
			
		}
	}
}

void AInvadedPlayer::SetBoundaries()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Boundaries"), Actors);
	if (Actors.Num() > 1)
	{
		FVector Location1 = Actors[0]->GetActorLocation();

		FVector Location2 = Actors[1]->GetActorLocation();

		BoundariesX.X = FMath::Min(Location1.X, Location2.X);
		BoundariesX.Y = FMath::Max(Location1.X, Location2.X);

		BoundariesY.X = FMath::Min(Location1.Y, Location2.Y);
		BoundariesY.Y = FMath::Max(Location1.Y, Location2.Y);

		BoundariesZ.X = FMath::Min(Location1.Z, Location2.Z);
		BoundariesZ.Y = FMath::Max(Location1.Z, Location2.Z);
	
	}
}

bool AInvadedPlayer::IsInsideBoundaries(FVector Value)
{
	if(Value.X>=BoundariesX.X&&Value.X<=BoundariesX.Y&&Value.Y >= BoundariesY.X&&Value.Y <= BoundariesY.Y&&Value.Z >= BoundariesZ.X&&Value.Z <= BoundariesZ.Y)
		return true;

	return false;
}

void AInvadedPlayer::SetFightCleanedServer_Implementation(bool Value)
{
	SetFightCleaned(Value);
}
bool AInvadedPlayer::SetFightCleanedServer_Validate(bool Value)
{
	return true;
}
void AInvadedPlayer::SetPlayerColorServer_Implementation(const FLinearColor & Value)
{
	SetPlayerColor(Value);
}
bool AInvadedPlayer::SetPlayerColorServer_Validate(const FLinearColor & Value)
{
	return true;
}
void AInvadedPlayer::PlayReverseFightAnim()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		AInvadedPC* InvadedPC = Cast<AInvadedPC>(GetController());
		if (InvadedPC)
		{
			AInvadedPCManager* InvadedPCManager = Cast<AInvadedPCManager>(InvadedPC->PlayerCameraManager);
			if (InvadedPCManager)
			{
				InvadedPCManager->PlayReverseAnim();
			}
			else
			{
				GetFightGameComponent()->EndGameReached();
			}
		}
		else
		{
			GetFightGameComponent()->EndGameReached();
		}
	}
	else
	{
		PlayReverseFightAnimClient();
	}
}
void AInvadedPlayer::PlayReverseFightAnimClient_Implementation()
{
	PlayReverseFightAnim();
}
bool AInvadedPlayer::PlayReverseFightAnimClient_Validate()
{
	return true;
}
void AInvadedPlayer::PlayFightAnim_Implementation(AInvadedCharacter* Enemy)
{
	AInvadedPC* InvadedPC = Cast<AInvadedPC>(GetController());
	if (InvadedPC)
	{
		AInvadedPCManager* InvadedPCManager = Cast<AInvadedPCManager>(InvadedPC->PlayerCameraManager);
		if (InvadedPCManager&&Enemy)
		{
			InvadedPCManager->PlayFightAnim(TopDownCameraComponent,this,Enemy);
		}
	}
}
bool AInvadedPlayer::PlayFightAnim_Validate(AInvadedCharacter* Enemy)
{
	return true;
}

void AInvadedPlayer::AddGameUIClient_Implementation()
{
	AddGameUI();
}
bool AInvadedPlayer::AddGameUIClient_Validate()
{
	return true;
}
void AInvadedPlayer::AddGameUI()
{
	if (Role < ROLE_Authority||IsLocallyControlled())
	{	
		if (LoadingWidget)
		{
			LoadingWidget->RemoveFromParent();
		}
			
		
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
		{
			UUserWidget* LiveWidget = *Itr;


			if (!LiveWidget->GetWorld())
			{
				continue;
			}
			else if(LiveWidget!=InvadedStateMachine)
			{
				if(!LiveWidget->IsA<UInvadedStateMachine>()&&!LiveWidget->IsA<UInvadedMenuState>())
				LiveWidget->RemoveFromParent();
			}
		}
		if (InvadedStateMachine)
		{
			InvadedStateMachine->AddToViewport();
		}
	}
	else
	{
		AddGameUIClient();
	}
}
void AInvadedPlayer::AddEndGameUIClient_Implementation()
{
	AddEndGameUI();
}
bool AInvadedPlayer::AddEndGameUIClient_Validate()
{
	return true;
}
void AInvadedPlayer::AddEndGameUI()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (InvadedStateMachineClass)
		{
			InvadedStateMachine->PushState(TEXT("UIWinner"));
		}
	}
	else
	{
		AddEndGameUIClient();
	}
}
void AInvadedPlayer::GameLost()
{
	Super::GameLost();

	InvadedMovementComp->Deactivate();

	RemoveWidgets();
	
	AInvadedPC* InvadedPC = Cast<AInvadedPC>(GetController());
	if (InvadedPC)
	{
		InvadedPC->Spectate();
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	//	Destroy();
	}
	
}
void AInvadedPlayer::GameWon()
{
	Super::GameWon();
	if (InvadedStateMachine)
	{
		if (InvadedStateMachine->FindStateByName(TEXT("GameUI")))
		{
			InvadedStateMachine->FindStateByName(TEXT("GameUI"))->SetVisibility(ESlateVisibility::Collapsed);
			BP_CollapseHUD();
		}
		InvadedStateMachine->PushState(TEXT("ScoreboardUI"));
		
	}
}

void AInvadedPlayer::Zoom_Implementation(const float & Amount)
{
	float StartZ = 0.0f;

	float NewAmount = Amount * UPlayerSettings::GetZoomSpeed();

	if (StartingCell)
	{
		StartZ=StartingCell->GetActorLocation().Z;
	}
	NewDesiredArmLength = FMath::Clamp(GetTopDownCameraComponent()->GetComponentLocation().Z + NewAmount * AGridGenerator::GetMapScale(), StartZ+MinZoom*AGridGenerator::GetMapScale(), StartZ+GetActorLocation().Z+MaxZoom * AGridGenerator::GetMapScale());
	FVector Location =GetTopDownCameraComponent()->GetComponentLocation();
	Location.Z = NewDesiredArmLength;
	if (IsInsideBoundaries(Location))
	{
		while (!(FMath::IsNearlyEqual(GetTopDownCameraComponent()->GetComponentLocation().Z, NewDesiredArmLength, 0.1f)))
		{
			GetTopDownCameraComponent()->SetWorldLocation(FVector(GetTopDownCameraComponent()->GetComponentLocation().X, GetTopDownCameraComponent()->GetComponentLocation().Y, FMath::FInterpTo(GetTopDownCameraComponent()->GetComponentLocation().Z, NewDesiredArmLength, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.0f)));
		}
	}
	
}
void AInvadedPlayer::MoveForward_Implementation(const float & Amount)
{
	if (!bLockCamera)
	{
		FVector Location;
		FVector Rotation = GetTopDownCameraComponent()->GetComponentRotation().Vector();
		FVector Input = Rotation;

		Location = Amount  * 15.0f*Input + GetTopDownCameraComponent()->GetComponentLocation();
		Location.Z = GetTopDownCameraComponent()->GetComponentLocation().Z;

		if (IsInsideBoundaries(Location))
			GetTopDownCameraComponent()->SetWorldLocation(Location);
	}

}
void AInvadedPlayer::MoveRight_Implementation(const float & Amount)
{
	if (!bLockCamera)
	{
		FVector Location;
		FVector Rotation = GetTopDownCameraComponent()->GetComponentRotation().Vector();
		FVector Input = FRotationMatrix(GetTopDownCameraComponent()->GetComponentRotation()).GetScaledAxis(EAxis::Y);

		Location = Amount * 15.0f*Input + GetTopDownCameraComponent()->GetComponentLocation();
		Location.Z = GetTopDownCameraComponent()->GetComponentLocation().Z;

		if (IsInsideBoundaries(Location))
			GetTopDownCameraComponent()->SetWorldLocation(Location);
	}
	
}
void AInvadedPlayer::Rotate_Implementation(const float & Amount)
{
	if (!bLockCamera)
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
	
	
}

void AInvadedPlayer::UseCard(ACardBase * Card)
{
	if (Role < ROLE_Authority)
	{
		UseCardServer(Card);
	}
	else
	{
		if (Card)
		{
			BP_UseCard(Card);

			Super::UseCard(Card);

			ClearSelections();
		}

	}
}
void AInvadedPlayer::AddFightUIClient_Implementation(const FString & StateID)
{
	AddFightUI(StateID);
}
bool AInvadedPlayer::AddFightUIClient_Validate(const FString & StateID)
{
	return true;
}
void AInvadedPlayer::AddFightUI(const FString& StateID)
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("StateID %s"), *StateID);
		if (InvadedStateMachine)
		{
			InvadedStateMachine->AddState(StateID);
		}

		CurrentFightStateID = StateID;
	}
	else
	{
		AddFightUIClient(StateID);
	}
	
}

void AInvadedPlayer::TogglePauseUI()
{
	if (PauseUI)
	{
		PauseUI->RemoveFromParent();
		PauseUI = nullptr;

		if (PlayerDeckUI)
		{
			PlayerDeckUI->SetVisibility(ESlateVisibility::Visible);
		}
		else if (InvadedStateMachine)
		{
			InvadedStateMachine->SetVisibility(ESlateVisibility::Visible);
			GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
		}
		
		
	}
	else if (PauseUIClass)
	{
		PauseUI = CreateWidget<UUserWidget>(GetWorld(),PauseUIClass);
		if (InvadedStateMachine)
		{
			InvadedStateMachine->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (PlayerDeckUI)
		{
			PlayerDeckUI->SetVisibility(ESlateVisibility::Collapsed);
		}
		PauseUI->AddToViewport();
		GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_UI);
	}
}

void AInvadedPlayer::TogglePlayerDeck()
{
	if (PlayerDeckUI)
	{
		PlayerDeckUI->RemoveFromParent();
		PlayerDeckUI = nullptr;
		if (PauseUI)
		{
			PauseUI->SetVisibility(ESlateVisibility::Visible);
		}
		else if (InvadedStateMachine)
		{
			InvadedStateMachine->SetVisibility(ESlateVisibility::Visible);
			GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
		}
	

	}
	else if (PlayerDeckUIClass)
	{
		PlayerDeckUI = CreateWidget<UUserWidget>(GetWorld(), PlayerDeckUIClass);
		if (InvadedStateMachine)
		{
			InvadedStateMachine->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (PauseUI)
		{
			PauseUI->SetVisibility(ESlateVisibility::Collapsed);
		}
		PlayerDeckUI->AddToViewport();
		GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_UI);
	}
}

void AInvadedPlayer::AddGameStartUI()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (InvadedStateMachine)
		{
			InvadedStateMachine->AddState("GameStart");
		}
	}
	else
	{
		AddGameStartUIClient();
	}
}

void AInvadedPlayer::RemoveGameStartUI()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (InvadedStateMachine)
		{
			InvadedStateMachine->RemoveState("GameStart");

			InvadedStateMachine->PushState("GameUI");
		}
	}
	else
	{
		RemoveGameStartUIClient();
	}
}

void AInvadedPlayer::RemoveFightUI(const FString & StateID)
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (InvadedStateMachine)
		{
			InvadedStateMachine->RemoveState(StateID);
		}
	}
	else
	{
		RemoveFightUIClient(StateID);
	}

}
void AInvadedPlayer::RemoveFightUIClient_Implementation(const FString & StateID)
{
	RemoveFightUI(StateID);
}
bool AInvadedPlayer::RemoveFightUIClient_Validate(const FString & StateID)
{
	return true;
}
void AInvadedPlayer::SetPlayerStrength(int32 Value)
{
	if (Role < ROLE_Authority)
	{
		SetPlayerStrengthServer(Value);
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			InvadedGM->SetPlayerFightValue(this, Value);
		}
	}
}
void AInvadedPlayer::SetPlayerStrengthServer_Implementation(int32 Value)
{
	SetPlayerStrength(Value);
}
bool AInvadedPlayer::SetPlayerStrengthServer_Validate(int32 Value)
{
	return true;
}
void AInvadedPlayer::SetCameraLocation_Implementation(FVector Location)
{
	TopDownCameraComponent->SetWorldLocation(Location);
}
bool AInvadedPlayer::SetCameraLocation_Validate(FVector Location)
{
	return true;
}
void AInvadedPlayer::SetCameraRotation_Implementation(FRotator Rotation)
{
	TopDownCameraComponent->SetWorldRotation(Rotation);

	APlayerController* PlayerCon = Cast<APlayerController>(GetController());
	if (PlayerCon)
		PlayerCon->SetControlRotation(Rotation);
}
bool AInvadedPlayer::SetCameraRotation_Validate(FRotator Rotation)
{
	return true;
}
void AInvadedPlayer::ClearLastRetracePath()
{
	if (IsPlayerControlled()&&LastRetracePath.Num())
	{
		if (Role < ROLE_Authority)
		{
			ClearLastRetracePathServer();
		}
		else
		{
			Super::ClearLastRetracePath();
		}
	}
	
}
void AInvadedPlayer::AddGameStartUIClient_Implementation()
{
	AddGameStartUI();
}
bool AInvadedPlayer::AddGameStartUIClient_Validate()
{
	return true;
}
void AInvadedPlayer::RemoveGameStartUIClient_Implementation()
{
	RemoveGameStartUI();
}
bool AInvadedPlayer::RemoveGameStartUIClient_Validate()
{
	return true;
}
void AInvadedPlayer::ClearLastPath()
{
	for (ACellBase* Cell : LastPath)
	{
		if (Cell)
		{
			Cell->SetHighlightState(EHighlightState::HC_Positive);
			Cell->OnEndFocus();
		}
		
	}
	//LastPath.Empty();

}
void AInvadedPlayer::LockCamera()
{
	bLockCamera = true;
}
void AInvadedPlayer::UnlockCamera()
{
	bLockCamera = false;
}
void AInvadedPlayer::FightWon_Implementation(AInvadedCharacter* Enemy)
{

	FightWon_BP();

	FightStatus = 1;

	TempEnemy = Enemy;
	Super::FightWon_Implementation(Enemy);
	
}
void AInvadedPlayer::FightLost_Implementation(AInvadedCharacter* Enemy)
{
	Super::FightLost_Implementation(Enemy);

	FightLost_BP();
	FightStatus = -1;

	TempEnemy = Enemy;
}
void AInvadedPlayer::FightDraw_Implementation(AInvadedCharacter* Enemy)
{
	Super::FightDraw_Implementation(Enemy);
	FightStatus = 0;

	FightDraw_BP();

	TempEnemy = Enemy;
	
}
void AInvadedPlayer::ToggleDicePod()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (InvadedStateMachine)
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
void AInvadedPlayer::ToggleDicePodClient_Implementation()
{
	ToggleDicePod();
		
}
bool AInvadedPlayer::ToggleDicePodClient_Validate()
{
	return true;
}

void AInvadedPlayer::AddCard(class ACardBase* Value)
{
	if (Role < ROLE_Authority)
	{
		AddCardServer(Value);
	}
	else
	{
		CardsArray.Add(Value);

		FCardInfo CardInfo;
		CardInfo.CardName = Value->GetCardName();
		CardInfo.Texture = Value->GetTexture();

		AddCardPopUp(CardInfo);

	}
}
void AInvadedPlayer::PushState(const FString & StateID)
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		if (InvadedStateMachine)
		{

			InvadedStateMachine->PushState(StateID);
		}
	}
	else
	{
		PushStateClient(StateID);
	}
}
void AInvadedPlayer::PushStateClient_Implementation(const FString & StateID)
{
	PushState(StateID);
}
bool AInvadedPlayer::PushStateClient_Validate(const FString & StateID)
{
	return true;
}
void AInvadedPlayer::AddCardPopUp(const FCardInfo& CardInfo)
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		APlayerController* PlayerCon = Cast<APlayerController>(GetController());

		if (PlayerCon)
		{
			APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerCon->GetHUD());
			if (PlayerHUD)
			{
				PlayerHUD->Update();

				PlayerHUD->OpenNotificationBox(FText::FromString(CardInfo.CardName), 3.0f);
			}
		}
	}
	else
	{
		AddCardPopUpClient(CardInfo);
	}	
}
void AInvadedPlayer::AddPopup(const FString& Value)
{
	if (IsLocallyControlled())
	{
		/*
		AsyncTask(ENamedThreads::AnyThread, [=] {

			APlayerController* PlayerCon = Cast<APlayerController>(GetController());

			if (PlayerCon)
			{
				APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerCon->GetHUD());
				if (PlayerHUD)
				{
					PlayerHUD->Update();

					PlayerHUD->OpenNotificationBox(FText::FromString(Value), 3.0f);
				}
			}
		});
		*/
	
	}
	else
	{
	//	AddPopupClient(Value);
	}
	
}
void AInvadedPlayer::AddFightPopup(const FString& Player1, const FString& Player2)
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		/*
		APlayerController* PlayerCon = Cast<APlayerController>(GetController());

		if (PlayerCon)
		{
			APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerCon->GetHUD());
			if (PlayerHUD)
			{
				PlayerHUD->Update();

				PlayerHUD->OpenNotificationBox(FText::FromString(Player1 + " vs " + Player2), 3.0f);
			}
		}
		*/
	}
	else
	{
	//	AddFightPopupClient(Player1, Player2);
	}
	
}
void AInvadedPlayer::RemoveCardPopup()
{
	if (InvadedStateMachine)
	{
		//InvadedStateMachine->RemoveState(FString("UICardPopUp"));
	}
}
void AInvadedPlayer::InitFightGame()
{
	if (Role < ROLE_Authority)
	{
		InitFightGameServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			if (InvadedGM->GetCurrentFightGame())
			{
				InvadedGM->GetCurrentFightGame()->InitReached(GetInvadedPS());
			}			
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("InitFightGame Fail"));
				
				FTimerHandle ErrorHandle;
				GetWorld()->GetTimerManager().SetTimer(ErrorHandle, this, &AInvadedPlayer::InitFightGame, 0.5f, false);
			}
		}
		
	}
}
void AInvadedPlayer::InitFightGameServer_Implementation()
{
	InitFightGame();
}
bool AInvadedPlayer::InitFightGameServer_Validate()
{
	return true;
}

void AInvadedPlayer::AddCardPopUpClient_Implementation(const FCardInfo & CardInfo)
{
	AddCardPopUp(CardInfo);
}
bool AInvadedPlayer::AddCardPopUpClient_Validate(const FCardInfo & CardInfo)
{
	return true;
}
void AInvadedPlayer::AddFightPopupClient_Implementation(const FString & Player1, const FString & Player2)
{
	AddFightPopup(Player1, Player2);
}
bool AInvadedPlayer::AddFightPopupClient_Validate(const FString & Player1, const FString & Player2)
{
	return true;
}
void AInvadedPlayer::AddPopupClient_Implementation(const FString & Value)
{
	AddPopup(Value);
}
bool AInvadedPlayer::AddPopupClient_Validate(const FString & Value)
{
	return true;
}