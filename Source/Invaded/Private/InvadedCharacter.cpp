// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedCharacter.h"
#include "Player/InvadedMovementComp.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/InvadedGM.h"
#include "Actors/CellBase.h"
#include "Actors/Cards/CardBase.h"
#include "GameModes/InvadedGS.h"
#include "GameModes/InvadedData.h"
#include "Alg/AStar.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/Material.h"
#include "Actors/CellCorner.h"
#include "Player/InvadedPlayer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "FightGameComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/StaticMeshComponent.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "Engine/LocalPlayer.h"

void AInvadedCharacter::SetHighlightState_Implementation(EHighlightState Value)
{
	switch (Value)
	{
	case EHighlightState::HC_Positive:
	{
		for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* Inst = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(i));
			if (Inst)
			{
				Inst->SetVectorParameterValue("GlowColor", PositiveColor);
			}
		}
		break;
	}
	case EHighlightState::HC_Negative:
	{
		for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* Inst = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(i));
			if (Inst)
			{
				Inst->SetVectorParameterValue("GlowColor", NegativeColor);
			}
		}
		break;
	}
	}
}

// Sets default values
AInvadedCharacter::AInvadedCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UInvadedMovementComp>(ACharacter::CharacterMovementComponentName))
{

	InvadedPS = CreateDefaultSubobject<UInvadedData>(TEXT("InvadedPS"));
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

	FightGameComponent = CreateDefaultSubobject<UFightGameComponent>(TEXT("FightGameComponent"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;

	//GetMesh()->SetCustomDepthStencilValue(250);

	SM_Rifle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Rifle"));


	SM_Rifle->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "WeaponSlot");

}

void AInvadedCharacter::GameLost()
{
	GetInvadedMovementComp()->PlayMontage(EMontageType::MT_Death);

	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGS)
		InvadedGS->SetNumberOfAlivePlayers(InvadedGS->GetNumberOfAlivePlayers()-1);

	if (InvadedGS->GetCurrentPlayer() == InvadedPS)
		CallTurnEnd();
}
void AInvadedCharacter::GameWon()
{
	GetInvadedMovementComp()->PlayMontage(EMontageType::MT_Win);
}
void AInvadedCharacter::DropFlag()
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		DropFlagServer();
	}
	else
	{	
		for (AFlag* Flag : Flags)
		{
			if (Flag->GetCornerCell()==GetOversteppedCell())
			{
				UE_LOG(LogTemp, Warning, TEXT("Drop"));
				Flag->RemoveFromPlayer();
			}
		}	
	}
	
}
void AInvadedCharacter::DropFlagServer_Implementation()
{
	DropFlag();
}
bool AInvadedCharacter::DropFlagServer_Validate()
{
	return true;
}
void AInvadedCharacter::ToggleDicePod()
{
}
bool AInvadedCharacter::ProjectPath(int32 Distance, TArray<ACellBase*> CellPath)
{
	UE_LOG(LogTemp, Warning, TEXT("Distance  %d"), Distance);
	GetInvadedPS()->SetRemainingMoves(Distance);
	LastPath = CellPath;
	RPath = CellPath;
	RetraceThePath();
	
	return true;
	
}
void AInvadedCharacter::KilledPlayer()
{
	//RPath.Empty();
	for (ACellBase* Cell : LastRetracePath) 
	{
		ChangeCellHighlight(Cell, false, EHighlightState::HC_Positive);
	}
}
void AInvadedCharacter::UpdateCellMaterialServer_Implementation(class ACellBase* Cell, const FLinearColor & BaseColor, EHighlightState HighlightState)
{
	UpdateCellMaterialMulitcast(Cell, BaseColor, HighlightState);
}
bool AInvadedCharacter::UpdateCellMaterialServer_Validate(class ACellBase* Cell, const FLinearColor & BaseColor, EHighlightState HighlightState)
{
	return true;
}
void AInvadedCharacter::UpdateCellMaterial(class ACellBase* Cell, const FLinearColor& BaseColor, EHighlightState HighlightState)
{
	if (Role < ROLE_Authority)
	{
		UpdateCellMaterialServer(Cell, BaseColor, HighlightState);
	}
	else
	{
		UpdateCellMaterialMulitcast(Cell, BaseColor, HighlightState);
	}

}
void AInvadedCharacter::UpdateCellMaterialMulitcast_Implementation(class ACellBase* Cell, const FLinearColor & BaseColor, EHighlightState HighlightState)
{
	
	if (Cell)
	{
		Cell->UpdateCellMaterial(BaseColor, HighlightState);
	}
	
	
}
bool AInvadedCharacter::UpdateCellMaterialMulitcast_Validate(class ACellBase* Cell, const FLinearColor & BaseColor, EHighlightState HighlightState)
{
	return true;
}
void AInvadedCharacter::ForceChangeCellHighlight_Implementation(ACellBase * Cell, bool bSwitch, EHighlightState HighlightState)
{
	if (Cell)
	{
		Cell->SetIsFocusable(true);

		Cell->SetHighlightState(HighlightState);
		if (bSwitch)
			Cell->OnBeginFocus();
		else
			Cell->OnEndFocus();
	}
}
bool AInvadedCharacter::ForceChangeCellHighlight_Validate(ACellBase * Cell, bool bSwitch, EHighlightState HighlightState)
{
	return true;
}
void AInvadedCharacter::ChangePlayerHighlightMulticast_Implementation(AInvadedCharacter * Player, bool bSwitch, EHighlightState HighlightState)
{
	if (Player)
	{
		Player->SetHighlightState(HighlightState);
		if (bSwitch)
			Player->OnBeginFocus();
		else
			Player->OnEndFocus();
	}
}
bool AInvadedCharacter::ChangePlayerHighlightMulticast_Validate(AInvadedCharacter * Player, bool bSwitch, EHighlightState HighlightState)
{
	return true;
}
void AInvadedCharacter::ChangePlayerHighlight(AInvadedCharacter * Player, bool bSwitch, EHighlightState HighlightState)
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		ChangePlayerHighlightServer(Player, bSwitch, HighlightState);
	}
	else
	{
		ChangePlayerHighlightMulticast(Player, bSwitch, HighlightState);
	}
}
void AInvadedCharacter::ChangePlayerHighlightServer_Implementation(AInvadedCharacter * Player, bool bSwitch, EHighlightState HighlightState)
{
	ChangePlayerHighlightMulticast(Player, bSwitch, HighlightState);
}
bool AInvadedCharacter::ChangePlayerHighlightServer_Validate(AInvadedCharacter * Player, bool bSwitch, EHighlightState HighlightState)
{
	return true;
}
void AInvadedCharacter::TeleportPlayer(AInvadedCharacter * Player, FVector Location)
{
	if (Role < ROLE_Authority && IsLocallyControlled())
	{
		TeleportPlayerServer(Player,Location);
	}
	else
	{
		TeleportPlayerMulticast(Player, Location);
	}
}
void AInvadedCharacter::TeleportPlayerServer_Implementation(AInvadedCharacter * Player, FVector Location)
{
	TeleportPlayerMulticast(Player, Location);
}
bool AInvadedCharacter::TeleportPlayerServer_Validate(AInvadedCharacter * Player, FVector Location)
{
	return true;
}
void AInvadedCharacter::TeleportPlayerMulticast_Implementation(AInvadedCharacter * Player, FVector Location)
{
	Player->TeleportTo(Location, Player->GetActorRotation());
}
bool AInvadedCharacter::TeleportPlayerMulticast_Validate(AInvadedCharacter * Player, FVector Location)
{
	return true;
}
// Called when the game starts or when spawned
void AInvadedCharacter::BeginPlay()
{
	Super::BeginPlay();
	InvadedMovementComp = Cast<UInvadedMovementComp>(GetCharacterMovement());
	AnimInstance = Cast<UInvadedAnimInstance>(GetMesh()->GetAnimInstance());

	if (bSetColorVariables&&InvadedPS)
	{
		InvadedPS->SetPlayerColor(PlayerColor);
		InvadedPS->SetFlagColor(FlagColor);
	}

	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* Inst = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(i), this);

		Inst->SetVectorParameterValue("P_Color1", InvadedPS->GetPlayerColor());

		Inst->SetVectorParameterValue("P_Color2", InvadedPS->GetFlagColor());

		GetMesh()->SetMaterial(i, Inst);

	}
	SetHighlightState(EHighlightState::HC_Positive);
	
	if (IsPlayerControlled())
	{
		//Cache all the available achievements
		QueryAchievements();
	}
}

void AInvadedCharacter::SetPathFound(bool  Value)
{
	bPathFound = Value;
}

void AInvadedCharacter::ThrowDice()
{
	if (Role < ROLE_Authority)
	{
		ThrowDiceServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM&&InvadedPS->GetIsCurrentPlayer()==true)
		{
			InvadedGM->ThrowDice();
		}
	}
}

void AInvadedCharacter::ThrowDiceServer_Implementation()
{
	ThrowDice();
}
bool AInvadedCharacter::ThrowDiceServer_Validate()
{
	return true;
}
void AInvadedCharacter::SetLastFocusedCell(ACellBase * Value)
{
	if (Role < ROLE_Authority)
	{
		SetLastFocusedCellServer(Value);
	}
	else
	{
		LastFocusedCell = Value;
	}
	
}
void AInvadedCharacter::SetLastFocusedCellServer_Implementation(ACellBase * Value)
{
	SetLastFocusedCell(Value);
}
bool AInvadedCharacter::SetLastFocusedCellServer_Validate(ACellBase * Value)
{
	return true;
}
void AInvadedCharacter::SetLastFocusedPlayer(AInvadedCharacter * Value)
{
	if (Role < ROLE_Authority)
	{
		SetLastFocusedPlayerServer(Value);
	}
	else
	{
		LastFocusedPlayer = Value;
	}
	
}
void AInvadedCharacter::SetLastFocusedPlayerServer_Implementation(AInvadedCharacter * Value)
{
	SetLastFocusedPlayer(Value);
}
bool AInvadedCharacter::SetLastFocusedPlayerServer_Validate(AInvadedCharacter * Value)
{
	return true;
}
void AInvadedCharacter::SetOversteppedCell(ACellBase * Value)
{
	if (Role < ROLE_Authority)
	{
		SetOversteppedCellServer(Value);
	}
	else
	{
		if (OversteppedCell)
		{
			DeoccupyCell(OversteppedCell);
		}
		OversteppedCell = Value;
		if (OversteppedCell)
		{
		
				OversteppedCell->SetIsOccupied(true);
				OversteppedCell->SetOccupyingPlayer(this);
				for (AFlag* Flag : Flags)
				{
					if (Flag)
					{
						if (Flag->GetFirstOwner() != this && Flag->GetDeathCell() == OversteppedCell)
						{
							DestroyFlag(Flag);
						}
					}

				};
			
			
			
		}
	}
	
}
void AInvadedCharacter::SetOversteppedCellServer_Implementation(ACellBase * Value)
{
	SetOversteppedCell(Value);
}
bool AInvadedCharacter::SetOversteppedCellServer_Validate(ACellBase* Value)
{
	return true;
}

void AInvadedCharacter::CallTurnEnd()
{
	
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		CallTurnEndServer();
	}
	else
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

		ClearSelections();
		ClearLastRetracePath();

		if (InvadedPS->GetRemainingMoves() == 0 && InvadedPS->HasThrownDice()&&!InvadedPS->GetAdditionalThrow())
		{
			AInvadedGM* InvadedGM = Cast<AInvadedGM>(GetWorld()->GetAuthGameMode());
			AInvadedGS* InvadedGS = Cast<AInvadedGS>(GetWorld()->GetGameState());
			if (InvadedGM&&InvadedGS->GetCurrentPlayer() == InvadedPS)
			{
				InvadedPS->SetIsCurrentPlayer(false);
				InvadedGM->OnEndTurn();
			}
		}
		
		
		
	}
	
}
void AInvadedCharacter::ForceCallTurnEnd()
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		ForceCallTurnEndServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(GetWorld()->GetAuthGameMode());
		AInvadedGS* InvadedGS = Cast<AInvadedGS>(GetWorld()->GetGameState());
		if (InvadedGM&&InvadedGS->GetCurrentPlayer() == InvadedPS)
		{
			InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

			ClearSelections();
			ClearLastRetracePath();

			InvadedPS->SetIsCurrentPlayer(false);
			InvadedGM->OnEndTurn();
		}
	}	
}
bool AInvadedCharacter::ForceCallTurnEndServer_Validate()
{
	return true;
}
void AInvadedCharacter::ForceCallTurnEndServer_Implementation()
{
	ForceCallTurnEnd();
}
void AInvadedCharacter::CallTurnEndServer_Implementation()
{
	CallTurnEnd();
}
bool AInvadedCharacter::CallTurnEndServer_Validate()
{
	return true;
}
void AInvadedCharacter::HighlightCell()
{
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{

		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(ECC_Camera, true, TraceHitResult);
		FVector CursorFV = TraceHitResult.ImpactNormal;
		FRotator CursorR = CursorFV.Rotation();




		if (TraceHitResult.GetActor())
		{
			
			ACellBase* FocusedCell = Cast<ACellBase>(TraceHitResult.GetActor());
			if (FocusedCell&&FocusedCell != LastFocusedCell)
			{
				if (FocusedCell->IsFocusable())
				{
					if (LastFocusedCell)
					{
						if (!InvadedPS->GetIsCurrentPlayer())
							LastFocusedCell->OnEndFocus();
						else if (!LastPath.Contains(LastFocusedCell))
							LastFocusedCell->OnEndFocus();
						
					}
					SetLastFocusedCell(FocusedCell);
					LastFocusedCell = FocusedCell;


					if (LastFocusedCell)
						LastFocusedCell->OnBeginFocus();
				}


			}
		}

	}
}

void AInvadedCharacter::HighlightCornerCell()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{

		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(ECC_Camera, true, TraceHitResult);
		FVector CursorFV = TraceHitResult.ImpactNormal;
		FRotator CursorR = CursorFV.Rotation();




		if (TraceHitResult.GetActor())
		{


			ACellBase* FocusedCell = Cast<ACellBase>(TraceHitResult.GetActor());
			if (FocusedCell&&FocusedCell != LastFocusedCell)
			{
				if (FocusedCell->IsFocusable())
				{
					if (LastFocusedCell && !LastPath.Contains(LastFocusedCell))
					{
						if (TraceHitResult.GetActor()->IsA<ACellCorner>() == false)
						{
							LastFocusedCell->SetHighlightState(EHighlightState::HC_Positive);
						}
						LastFocusedCell->OnEndFocus();
					}
					SetLastFocusedCell(FocusedCell);
					LastFocusedCell = FocusedCell;


					if (LastFocusedCell)
					{
						if (TraceHitResult.GetActor()->IsA<ACellCorner>() == false)
						{
							LastFocusedCell->SetHighlightState(EHighlightState::HC_Negative);
						}
						LastFocusedCell->OnBeginFocus();
					}

				}



			}

		}

	}
}

void AInvadedCharacter::HighlightPlayer()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(ECC_Camera, true, TraceHitResult);
		FVector CursorFV = TraceHitResult.ImpactNormal;
		FRotator CursorR = CursorFV.Rotation();




		if (TraceHitResult.GetActor())
		{
			

			AInvadedCharacter* FocusedPlayer = Cast<AInvadedCharacter>(TraceHitResult.GetActor());
			if (FocusedPlayer&&FocusedPlayer != LastFocusedPlayer)
			{
				
				if (FocusedPlayer != this)
				{
					if (LastFocusedPlayer)
					{
						ChangePlayerHighlight(LastFocusedPlayer, false, EHighlightState::HC_Positive);
					}
					SetLastFocusedPlayer(FocusedPlayer);
					LastFocusedPlayer = FocusedPlayer;
					if (LastFocusedPlayer)
					{
						ChangePlayerHighlight(LastFocusedPlayer, true, EHighlightState::HC_Positive);
					}

				}
			}

		}

	}
}

void AInvadedCharacter::NewCellPath(ACellBase * Cell)
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		NewCellPathServer(Cell);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("New Cell Path %s"),*Cell->GetName());
		if (Cell)
		{
			
			if (Cell->GetIsOccupied() && Cell->GetOccupyingPlayer() != this)
			{
				AInvadedCharacter* OtherPlayer = Cast<AInvadedCharacter>(Cell->GetOccupyingPlayer());
				if (OtherPlayer)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player Fight %s"), *OtherPlayer->GetInvadedPS()->GetPlayerName());
					if (GetWorld()->GetAuthGameMode<AInvadedGM>()->StartFight(GetInvadedPS(), OtherPlayer->GetInvadedPS()))
					{
						InvadedPS->SetIsAttacker(true);
						SetPlayerMode(EPlayerMode::PM_Fight);
						EnterFightMode(OtherPlayer);
						OtherPlayer->SetPlayerMode(EPlayerMode::PM_Fight);
						OtherPlayer->EnterFightMode(this);
						GetInvadedMovementComp()->SetMoveStatus(EMoveStatus::MS_Idle);
						GetInvadedMovementComp()->SetIsMoving(false);

					}	
				}
				else
				{
					Cell->OnInteract(this);
					SetOversteppedCell(Cell);
				}

			}
			else
			{
				Cell->OnInteract(this);
				SetOversteppedCell(Cell);
				
				
			}
		}
		
	}
}

void AInvadedCharacter::NewCellPathServer_Implementation(ACellBase * Cell)
{
	NewCellPath(Cell);
}
bool AInvadedCharacter::NewCellPathServer_Validate(ACellBase * Cell)
{
	return true;
}
void AInvadedCharacter::DeoccupyCell(ACellBase * Cell)
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		DeoccupyCellServer(Cell);
	}
	else
	{
		if (Cell)
		{
		

			if (Cell->GetOccupyingPlayer() == nullptr || Cell->GetOccupyingPlayer() == this) // in case of winning/losing fight
			{
				Cell->SetIsOccupied(false);
				Cell->SetOccupyingPlayer(nullptr);
			}
			
		}
	}
}

void AInvadedCharacter::DeoccupyCellServer_Implementation(ACellBase * Cell)
{
	DeoccupyCell(Cell);
}
bool AInvadedCharacter::DeoccupyCellServer_Validate(ACellBase * Cell)
{
	return true;
}
void AInvadedCharacter::RotatePlayer(float  Rotation)
{
	if (Role < ROLE_Authority)
	{
		RotatePlayerServer(Rotation);
	}
	else
	{
		RotatePlayerMulticast(Rotation);
	}
}

void AInvadedCharacter::RotatePlayerServer_Implementation(float  Rotation)
{
	RotatePlayerMulticast(Rotation);
}
bool AInvadedCharacter::RotatePlayerServer_Validate(float  Rotation)
{
	return true;
}
void AInvadedCharacter::RotatePlayerMulticast_Implementation(float  Rotation)
{
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
}
bool AInvadedCharacter::RotatePlayerMulticast_Validate(float  Rotation)
{
	return true;
}
void AInvadedCharacter::UseCard(ACardBase * Card)
{
	if (Role < ROLE_Authority)
	{
		UseCardServer(Card);
	}
	else
	{
		if (Card&&InvadedPS->GetIsCurrentPlayer())
		{
			
			FCardInfo CardInfo;
			CardInfo.CardName = Card->GetCardName();
			CardInfo.Texture = Card->GetTexture();
					
			Card->OnInteract(this);

			LastUsedCard = Card;
			
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);
			for (AActor* Actor : Actors)
			{

				AInvadedPlayer* Player = Cast<AInvadedPlayer>(Actor);
				
				if (Player)
				{
					Player->AddCardPopUp(CardInfo);
				}
			}
			
			CardsArray.Remove(Card);
			
			ClearSelections();
			
			
		}
		
	}
}

void AInvadedCharacter::UseCardServer_Implementation(ACardBase * Card)
{
	UseCard(Card);
}
bool AInvadedCharacter::UseCardServer_Validate(ACardBase * Card)
{
	return true;
}

void AInvadedCharacter::RetraceThePath()
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		RetraceThePathServer();
	}
	else
	{
		bThrownDice = true;
		RPath = LastPath;
		LastRetracePath = RPath;
		HighlightPath();
		NextPathCell();
	}
}

void AInvadedCharacter::EnterFightMode(AActor* Enemy)
{

	if (InvadedPS)
	{
		//InvadedPS->SetPlayerMode(EPlayerMode::PM_Fight);
	}
	FVector VDist = GetActorLocation() - Enemy->GetActorLocation();
	VDist.Normalize();
	FVector ForwardVector = GetCapsuleComponent()->GetForwardVector();

	float Rotation;
	if (FMath::IsNearlyEqual(VDist.X, ForwardVector.X, 0.1f) && FMath::IsNearlyEqual(VDist.Y, ForwardVector.Y, 0.1f))
	{
		Rotation = 180.0f;
	}
	else if (FMath::IsNearlyEqual(VDist.X*-1.0f, ForwardVector.X, 0.1f) && FMath::IsNearlyEqual(VDist.Y*-1.0f, ForwardVector.Y, 0.1f))
	{
		Rotation = 0.0f;
	}
	else if (FMath::IsNearlyEqual(ForwardVector.X, 1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.Y, 1.0f, 0.1f) || FMath::IsNearlyEqual(ForwardVector.X, -1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.Y, -1.0f, 0.1f) || FMath::IsNearlyEqual(ForwardVector.Y, 1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.X, -1.0f, 0.1f) || FMath::IsNearlyEqual(ForwardVector.Y, -1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.X, 1.0f, 0.1f))
	{
		Rotation = -90.0f;
	}
	else
	{
		Rotation = 90.0f;
	}
	GetInvadedMovementComp()->SetMoveStatus(EMoveStatus::MS_Idle);
	GetInvadedMovementComp()->SetTurnOnly(true);
	GetInvadedMovementComp()->Turn(Rotation);

}

void AInvadedCharacter::SetStartingCell(ACellBase * Value)
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		SetStartingCellServer(Value);
	}
	else
	{
		StartingCell = Value;

		if (StartingCell)
		{
			StartingCell->SetBaseColor(InvadedPS->GetPlayerColor());

			UpdateCellMaterialMulitcast(StartingCell, InvadedPS->GetPlayerColor(), EHighlightState::HC_Positive);
		}

	//	TeleportTo(Value->GetActorLocation(),GetActorRotation());
	}
}
void AInvadedCharacter::SetStartingCellServer_Implementation(ACellBase * Value)
{
	SetStartingCell(Value);
}
bool AInvadedCharacter::SetStartingCellServer_Validate(ACellBase * Value)
{
	return Value;
}

void AInvadedCharacter::RetraceThePathServer_Implementation()
{
	RetraceThePath();
}
bool AInvadedCharacter::RetraceThePathServer_Validate()
{
	return true;
}
void AInvadedCharacter::DestroyFlagServer_Implementation(class AFlag* Value)
{
	DestroyFlag(Value);
}
bool AInvadedCharacter::DestroyFlagServer_Validate(class AFlag* Value)
{
	return true;
}
void AInvadedCharacter::DestroyFlag(class AFlag* Value)
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		DestroyFlagServer(Value);
	}
	else
	{
	//	Flags.Remove(Value);
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			InvadedGM->DestroyFlag(Value, Value->GetFirstOwner(),this);
		}
	}
}

void AInvadedCharacter::RemoveFlag(AFlag * Value)
{
	Flags.Remove(Value);
}

void AInvadedCharacter::NextPathCell()
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		 NextPathCellServer();
	}
	else
	{
		
		if (InvadedPS)
		{
			
			if (RPath.Num())
			{
				
				if (InvadedPS->GetRemainingMoves() > 0&&InvadedPS->GetPlayerMode()==EPlayerMode::PM_Base)
				{
					ACellBase* CurrentCell = RPath[0];
					if (CurrentCell)
					{
						if (CurrentCell->GetIsWalkable())
						{
							InvadedPS->SetRemainingMoves(InvadedPS->GetRemainingMoves() - 1);
							RPath.RemoveAt(0);
							NewCellPath(CurrentCell);
							
							
						}
						else
						{
							RPath.Empty();
							GetInvadedMovementComp()->ReachedDestination();
						}
					}
					else
					{
						RPath.Empty();
						GetInvadedMovementComp()->ReachedDestination();
					}

				}
				else if (InvadedPS->GetRemainingMoves() <= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("End Turn"));
					
					RPath.Empty();

					InvadedMovementComp->ReachedDestination();
					
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Next Path Cell Fighting"));
					InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
					InvadedMovementComp->ReachedDestination();
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Empty RPath"));
				InvadedMovementComp->ReachedDestination();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No PlayerState"));
			CallTurnEnd();
		}
	}

	
}


void AInvadedCharacter::NextPathCellServer_Implementation()
{
	return NextPathCell();
}
bool AInvadedCharacter::NextPathCellServer_Validate()
{
	return true;
}

void AInvadedCharacter::FindOversteppedCell()
{
	TArray<FHitResult> OutHitArray;

	FCollisionQueryParams Params;
	int32 FlagsNum = Flags.Num();
	if (FlagsNum)
	{
		for (int32 i = 0; i < FlagsNum; ++i)
		{
			Params.AddIgnoredActor(Flags[i]);
		}
	}	
	FCollisionResponseParams ResponseParam;
	if(GetWorld()&&GetActorLocation().Size())
	GetWorld()->LineTraceMultiByChannel(OutHitArray, GetActorLocation()+FVector(0.0f,0.0f,100.0f), GetActorLocation() - FVector(0.0f, 0.0f, 400.0f), ECollisionChannel::ECC_Visibility,Params);
	
	for (FHitResult OutHit : OutHitArray)
	{
		if (OutHit.GetActor())
		{
			ACellBase* Cell = Cast<ACellBase>(OutHit.GetActor());
			if (Cell)
			{
				
				SetOversteppedCell(Cell);
				

			}
		}
	}
}

void AInvadedCharacter::FindPath(ACellBase* StartCell,ACellBase* EndCell)
{

	
	if (FAStar::IsThreadFinished() && bPathFound == true && LastFocusedCell != LastTargetCell && InvadedMovementComp->IsMoving() == false&&InvadedPS)
	{
		
		AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
		if (InvadedPS->GetIsCurrentPlayer()&&InvadedPS->GetPlayerMode()==EPlayerMode::PM_Base)
		{
			
			AGridGenerator* GridGenerator = InvadedGS->GetGridGenerator();
			if (GridGenerator&&OversteppedCell&&LastFocusedCell)
			{
				
				Path.Empty();
				LastTargetCell = LastFocusedCell;
			
				if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_Base)
				{
					FAStar::JoyInit(OversteppedCell, LastFocusedCell, GridGenerator, Path);
				}
				else if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_Move&&LastFocusedPlayer)
				{
					FAStar::JoyInit(LastFocusedPlayer->GetOversteppedCell(), LastFocusedCell, GridGenerator, Path);
				}
				
				bPathFound = false;
				SetPathFound(false);

			}
		}

	}
}

void AInvadedCharacter::ClearLastRetracePath()
{	
	TArray<class ACellBase*> TempPath = LastRetracePath;
	for (int32 i = 0; i < TempPath.Num(); ++i)
	{
		if (TempPath[i])
		{
			TempPath[i]->SetIsFocusable(true);
			ForceChangeCellHighlight(TempPath[i], false, EHighlightState::HC_Positive);
		}
		
				
	}
}

void AInvadedCharacter::ClearLastRetracePathServer_Implementation()
{
	ClearLastRetracePath();
}
bool AInvadedCharacter::ClearLastRetracePathServer_Validate()
{
	return true;
}
// Called every frame
void AInvadedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (InvadedPS)
	{
	
		for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* Inst = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(i));
			if (Inst)
			{
				Inst->SetVectorParameterValue("P_Color1", InvadedPS->GetPlayerColor());

				Inst->SetVectorParameterValue("P_Color2", InvadedPS->GetFlagColor());
			}
		}


	}

}

void AInvadedCharacter::HighlightPath()
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		HighlightPathServer();
	}
	else
	{
		for (int32 i = 0; i < RPath.Num() && i < InvadedPS->GetRemainingMoves(); ++i)
		{
			ChangeCellHighlight(RPath[i], true, EHighlightState::HC_Special);
			RPath[i]->SetIsFocusable(false);
		}
	}
}

void AInvadedCharacter::HighlightPathServer_Implementation()
{
	HighlightPath();
}
bool AInvadedCharacter::HighlightPathServer_Validate()
{
	return true;
}
void AInvadedCharacter::ChangeCellHighlight_Implementation(ACellBase * Cell, bool bSwitch, EHighlightState HighlightState)
{

	if (Cell)
	{
		Cell->SetHighlightState(HighlightState);
		if (bSwitch)
			Cell->OnBeginFocus();
		else
			Cell->OnEndFocus();
	}
	
}
bool AInvadedCharacter::ChangeCellHighlight_Validate(ACellBase * Cell, bool bSwitch, EHighlightState HighlightState)
{
	return true;
}

void AInvadedCharacter::AddCard(ACardBase * Value)
{
	if (Role < ROLE_Authority)
	{
		AddCardServer(Value);

	}
	else
	{
		CardsArray.Add(Value);
	}
}

void AInvadedCharacter::AddCardServer_Implementation(ACardBase * Value)
{
	AddCard(Value);
}
bool AInvadedCharacter::AddCardServer_Validate(ACardBase * Value)
{
	return true;
}
void AInvadedCharacter::SetCurrentPath(const TArray<class ACellBase*>& Value)
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		SetCurrentPathServer(Value);
		
	}
	else
	{
		
		LastPath = Value;
	}
	
}
void AInvadedCharacter::SetCurrentPathServer_Implementation(const TArray<class ACellBase*>& Value)
{
	SetCurrentPath(Value);
}
bool AInvadedCharacter::SetCurrentPathServer_Validate(const TArray<class ACellBase*>& Value)
{
	return true;
}
void AInvadedCharacter::FightWon_Implementation(AInvadedCharacter* Enemy)
{

	InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

	InvadedMovementComp->SetTurnOnly(false);

	
	
}
void AInvadedCharacter::FightDraw_Implementation(AInvadedCharacter* Enemy)
{

	InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

	InvadedMovementComp->EndFight();

	InvadedMovementComp->SetMoveStatus(EMoveStatus::MS_Idle);


	
}
void AInvadedCharacter::FightLost_Implementation(AInvadedCharacter* Enemy)
{

	InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

	InvadedMovementComp->EndFight();

	InvadedMovementComp->SetMoveStatus(EMoveStatus::MS_Idle);

	

}
void AInvadedCharacter::OnBeginFocus_Implementation()
{
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* Inst = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(i));
		if (Inst)
		{
			Inst->SetScalarParameterValue("GlowOn", 1.0f);
		}
	}
	
}

void AInvadedCharacter::OnEndFocus_Implementation()
{
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* Inst = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(i));
		if (Inst)
		{
			Inst->SetScalarParameterValue("GlowOn", 0.0f);
		}
	}
}
void AInvadedCharacter::SetPlayerMode(EPlayerMode Value)
{
	if (Role < ROLE_Authority)
	{
		SetPlayerModeServer(Value);
	}
	else
	{
		
		if (InvadedPS)
		{
			InvadedPS->SetPlayerMode(Value);
		}
	}
}

void AInvadedCharacter::SetPlayerModeServer_Implementation(EPlayerMode Value)
{
	SetPlayerMode(Value);
}
bool AInvadedCharacter::SetPlayerModeServer_Validate(EPlayerMode Value)
{
	return true;
}
void AInvadedCharacter::RemoveFlags()
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		RemoveFlagsServer();
	}
	else
	{
		for (AFlag* Flag : Flags)
		{
			if (Flag)
			{
				if (Flag->GetParentActor() == this)
				{
					Flag->RemoveFromPlayer();
				}
			}
			
			
		}
		Flags.Empty();
		
	}
}
void AInvadedCharacter::RemoveFlagsServer_Implementation()
{
	RemoveFlags();
}
bool AInvadedCharacter::RemoveFlagsServer_Validate()
{
	return true;
}
void AInvadedCharacter::AddFlag(AFlag* Value)
{
	if (Value)
	{		
		if (Value->GetAttachParentActor())
			Value->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		Flags.Add(Value);

		if (FlagSockets.Num() >= Flags.Num())
		{
			Value->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FlagSockets[Flags.Num() - 1]);
		}
		else
		{
			Value->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, DefaultFlagSocketName);
		}


		Value->SetCurrentOwner(this);



		if (Value->GetFirstOwner() != this && Value->GetDeathCell() == OversteppedCell)
		{
			DestroyFlag(Value);
		}

		TArray<AActor*> Actors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);

		for (AActor* Actor : Actors)
		{
			AInvadedPlayer* Player = Cast<AInvadedPlayer>(Actor);
			if (Player)
			{

				Player->AddPopup("Flag picked up");

			}
		}

	}
		

	
	
}
void AInvadedCharacter::AddFlags(const TArray<AFlag*>& Array)
{
	if (Role < ROLE_Authority&&IsLocallyControlled())
	{
		AddFlagsServer(Array);
	}
	else
	{
		for (AFlag* Flag : Array)
		{
			AddFlag(Flag);
		}
	}
	
}
void AInvadedCharacter::AddFlagsServer_Implementation(const TArray<AFlag*>& Array)
{
	AddFlags(Array);
}
bool AInvadedCharacter::AddFlagsServer_Validate(const TArray<AFlag*>& Array)
{
	return true;
}
void AInvadedCharacter::ClearSelectionsClient_Implementation()
{
	ClearSelections();
}
bool AInvadedCharacter::ClearSelectionsClient_Validate()
{
	return true;
}
void AInvadedCharacter::ClearSelections()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		
		if (LastFocusedPlayer)
			LastFocusedPlayer->OnEndFocus();
		if (LastFocusedCell)
			LastFocusedCell->OnEndFocus();
			
		ClearLastPath();

	}
	else 
	{
		
		if (LastFocusedPlayer)
			ChangePlayerHighlight(LastFocusedPlayer, true, EHighlightState::HC_Neutral);
		if (LastFocusedCell)
			ChangeCellHighlight(LastFocusedCell, true, EHighlightState::HC_Neutral);
		


		ChangePlayerHighlight(LastFocusedPlayer, false, EHighlightState::HC_Neutral);

		ClearLastRetracePath();
		LastFocusedCell = nullptr;
		LastFocusedPlayer = nullptr;
		LastPath.Empty();
	}


}

void AInvadedCharacter::SetPlayerFightValue(int32 Value)
{
	if (Role < ROLE_Authority&&IsPlayerControlled())
	{
		SetPlayerFightValueServer(Value);
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

void AInvadedCharacter::SetPlayerFightValueServer_Implementation(int32 Value)
{
	SetPlayerFightValue(Value);
}
bool AInvadedCharacter::SetPlayerFightValueServer_Validate(int32 Value)
{
	return true;
}
void AInvadedCharacter::ClearLastPath()
{
	//Parent function
}
void AInvadedCharacter::QueryAchievements()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

			if (Achievements.IsValid())
			{
				//Cache all the game's achievements for future use and bind the OnQueryAchievementsComplete function to fire when we're finished caching
				Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &AInvadedCharacter::OnQueryAchievementsComplete));
			}
		}
	}
}

void AInvadedCharacter::OnQueryAchievementsComplete(const FUniqueNetId & PlayerId, const bool bWasSuccessful)
{

	if (bWasSuccessful)
	{
		GLog->Log("achievements were cached");
	}
	else
	{
		GLog->Log("failed to cache achievements");
	}
}

void AInvadedCharacter::UpdateAchievementProgress(FString Id, float Percent)
{
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();


			if (Achievements.IsValid() && (!AchievementsWriteObjectPtr.IsValid() || !AchievementsWriteObjectPtr->WriteState != EOnlineAsyncTaskState::InProgress))
			{
				//Make a shared pointer for achievement writing
				AchievementsWriteObjectPtr = MakeShareable(new FOnlineAchievementsWrite());

				//Sets the progress of the desired achievement - does nothing if the id is not valid
				AchievementsWriteObjectPtr->SetFloatStat(*Id, Percent);

				//Write the achievements progress
				FOnlineAchievementsWriteRef AchievementsWriteObjectRef = AchievementsWriteObjectPtr.ToSharedRef();
				Achievements->WriteAchievements(*UserId, AchievementsWriteObjectRef);
			}
		}
	}
}

void AInvadedCharacter::UnlockAchievement(FString Id)
{
	UpdateAchievementProgress(Id, 100.f);
}
void AInvadedCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	DOREPLIFETIME(AInvadedCharacter, bInitialized);
	DOREPLIFETIME(AInvadedCharacter, LastRetracePath);
	DOREPLIFETIME(AInvadedCharacter, RPath);
	DOREPLIFETIME(AInvadedCharacter, LastPath);
	DOREPLIFETIME(AInvadedCharacter, Path);
	DOREPLIFETIME(AInvadedCharacter, LastFocusedCell);
	DOREPLIFETIME(AInvadedCharacter, LastFocusedPlayer);
	DOREPLIFETIME(AInvadedCharacter, LastTargetCell);
	DOREPLIFETIME(AInvadedCharacter, OversteppedCell);
	DOREPLIFETIME(AInvadedCharacter, Flags);
	DOREPLIFETIME(AInvadedCharacter, CardsArray);
	DOREPLIFETIME(AInvadedCharacter, bThrownDice);
	DOREPLIFETIME(AInvadedCharacter, FightStatus);
	DOREPLIFETIME(AInvadedCharacter, LastUsedCard);
}