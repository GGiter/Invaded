// Fill out your copyright notice in the Description page of Project Settings.

#include "FightGameComponent.h"
#include "InvadedGM.h"
#include "Engine/World.h"
#include "InvadedData.h"
#include "MiniGames/WordGame.h"
#include "MiniGames/ClickGame.h"
#include "InvadedPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "InvadedStateMachine.h"
#include "GameWordWidget.h"
#include "InvadedMovementComp.h"

// Sets default values for this component's properties
UFightGameComponent::UFightGameComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;

	// ...
}


void UFightGameComponent::UpdateEnemyRemainingTries()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UpdateEnemyRemainingTriesServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame&&InvadedData)
			{
				int32 Value= FightGame->GetEnemyRemainingTries(InvadedData);
				if (Value>=0)
				{
					EnemyRemainingTries = FString::FromInt(Value);
				}
				else
				{
					EnemyRemainingTries = "0";
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyRemainingTries Call Fail"));
			}
		}
	}
}

void UFightGameComponent::UpdateEnemyName()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UpdateEnemyNameServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame&&InvadedData)
			{
				EnemyName = FightGame->GetEnemyName(InvadedData);		
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyName Call Fail"));
			}
		}
	}
}

void UFightGameComponent::UpdateEnemyScore()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UpdateEnemyScoreServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame&&InvadedData)
			{
				int32 Value = FightGame->GetEnemyScore(InvadedData);
				if (Value>=0)
				{
					EnemyScore = FString::FromInt(Value);
				}
				else
				{
					EnemyScore = "0";
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyScore Call Fail"));
			}
		}
	}
}

void UFightGameComponent::UpdatePlayerScore()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UpdatePlayerScoreServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame&&InvadedData)
			{
				Score = FString::FromInt(FightGame->GetPlayerScore(InvadedData));			
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UpdatePlayerScore Call Fail"));
			}
		}
	}
}

void UFightGameComponent::StartWordGame(int32 SentenceLength,int32 GameLengthValue, int32 WordGameHealthValue)
{
	for (int i = 0; i < SentenceLength; ++i)
	{
		NextSequence();
	}
	this->WordGameHealth = WordGameHealthValue;

	this->GameLength = GameLengthValue;
}

void UFightGameComponent::StartClickGame(int32 NumberOfStartTarget, int32 GameLenghtValue, int32 ClickGameHealthValue)
{
	for (int i = 0; i < NumberOfStartTarget; ++i)
	{
		NextTarget();
	}
	this->ClickGameHealth = ClickGameHealthValue;

	this->GameLength = GameLenghtValue;
}

void UFightGameComponent::FightWon(AInvadedCharacter * EnemyValue)
{
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	InvadedData->SetPlayerMode(EPlayerMode::PM_Base);
	if (InvadedCharacter)
	{
		InvadedCharacter->GetInvadedMovementComp()->PlayMontage(EMontageType::MT_ExitFight);
		if (InvadedData->GetIsCurrentPlayer())
		{
			bool bChecker = true;

			for (AFlag* Flag : InvadedCharacter->GetFlags())
			{
				if (Flag)
				{
					if (Flag->GetFirstOwner() != InvadedCharacter && Flag->GetDeathCell() == InvadedCharacter->GetOversteppedCell())
					{
						bChecker = false;
					}
				}
			}

			TArray<class AFlag*> NewFlags = EnemyValue->GetFlags();
			for (AFlag* Flag : NewFlags)
			{
				if (Flag)
				{
					if (Flag->GetFirstOwner() != InvadedCharacter && Flag->GetDeathCell() == InvadedCharacter->GetOversteppedCell())
					{
						bChecker = false;
					}
				}
			}
			EnemyValue->RemoveFlags();
			InvadedCharacter->AddFlags(NewFlags);


			if (bChecker)
				InvadedCharacter->GetInvadedMovementComp()->ReachedDestination();
		}
		else
		{
			InvadedCharacter->GetInvadedMovementComp()->ReachedDestination();
		}
	}

}

void UFightGameComponent::FightDraw(AInvadedCharacter * EnemyValue)
{
	InvadedData->SetPlayerMode(EPlayerMode::PM_Base);
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());



	if (InvadedCharacter)
	{
		
		if (InvadedData->GetIsCurrentPlayer())
		{
			InvadedData->SetRemainingMoves(0);

			InvadedCharacter->ClearSelections();
			InvadedCharacter->ClearLastRetracePath();
			InvadedCharacter->GetInvadedMovementComp()->ReachedDestination();
			//CallTurnEnd();


		}
		else
		{
			InvadedCharacter->ClearSelections();
			InvadedCharacter->ClearLastRetracePath();
			InvadedData->SetRemainingMoves(0);
		}

		
		InvadedCharacter->GetInvadedMovementComp()->PlayMontage(EMontageType::MT_Hit);
	}

	
	
}

void UFightGameComponent::FightLost(AInvadedCharacter * EnemyValue)
{
	InvadedData->SetPlayerMode(EPlayerMode::PM_Base);
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwner());
	if (InvadedCharacter)
	{
		

		if (InvadedData->GetIsCurrentPlayer())
		{
			InvadedData->SetRemainingMoves(0);

			InvadedCharacter->ClearSelections();
			InvadedCharacter->ClearLastRetracePath();

			InvadedCharacter->GetInvadedMovementComp()->ReachedDestination();
		}
		else
		{
			InvadedCharacter->ClearSelections();
			InvadedCharacter->ClearLastRetracePath();
			InvadedData->SetRemainingMoves(0);
		}

		InvadedCharacter->GetInvadedMovementComp()->PlayMontage(EMontageType::MT_Hit);
	}
	

}

void UFightGameComponent::EndGameReached()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		EndGameReachedServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{	
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame&&InvadedData)
			{
				FightGame->EndGameReached(InvadedData);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EndGameReaced Call Fail"));
			}
		}
	}
}
void UFightGameComponent::UpdateScore(int32 Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UpdateScoreServer(Value);
	}
	else
	{

		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame)
			{	
				FightGame->UpdateScore(InvadedData, Value);
			}
		}
	
	}
}
void UFightGameComponent::UpdateEnemy()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UpdateEnemyServer();
	}
	else
	{

		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AFightGame* FightGame = InvadedGM->GetCurrentFightGame();
			if (FightGame)
			{
				Enemy = FightGame->GetEnemy(InvadedData);
			}
		}

	}
}
void UFightGameComponent::UpdateData()
{
	UpdateEnemyRemainingTries();
	UpdateEnemyName();
	UpdateEnemyScore();
	UpdatePlayerScore();
	UpdateEnemy();
}
void UFightGameComponent::EndGameReachedServer_Implementation()
{
	EndGameReached();
}
bool UFightGameComponent::EndGameReachedServer_Validate()
{
	return true;
}
// Called when the game starts
void UFightGameComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerChar = Cast<AInvadedCharacter>(GetOwner());

	if (PlayerChar)
	{
		InvadedData = PlayerChar->GetInvadedPS();
	}

}

void UFightGameComponent::UpdateScoreServer_Implementation(int32 Value)
{
	UpdateScore(Value);
}
bool UFightGameComponent::UpdateScoreServer_Validate(int32 Value)
{
	return true;
}
void UFightGameComponent::NextTargetServer_Implementation()
{
	NextTarget();
}
bool UFightGameComponent::NextTargetServer_Validate()
{
	return true;
}
void UFightGameComponent::SentenceSucceedServer_Implementation(int32 Length)
{
	SentenceSucceed(Length);
}
bool UFightGameComponent::SentenceSucceedServer_Validate(int32 Length)
{
	return true;
}

void UFightGameComponent::SentenceFailServer_Implementation()
{
	SentenceFail();
}
bool UFightGameComponent::SentenceFailServer_Validate()
{
	return true;
}
void UFightGameComponent::TimeIsUpServer_Implementation()
{
	TimeIsUp();
}
bool UFightGameComponent::TimeIsUpServer_Validate()
{
	return true;
}

void UFightGameComponent::NextSequenceServer_Implementation()
{
	NextSequence();
}
bool UFightGameComponent::NextSequenceServer_Validate()
{
	return true;
}

// Called every frame
void UFightGameComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFightGameComponent::NextTarget()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		NextTargetServer();
	}
	else
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AClickGame* WordGame = Cast<AClickGame>(InvadedGM->GetCurrentFightGame());
			if (WordGame)
			{	
				TargetArray.Push(WordGame->GetNextTarget(InvadedData));
			}
		}
	}
}

void UFightGameComponent::NextSequence()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		NextSequenceServer();
	}
	else
	{
		
			AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
			if (InvadedGM)
			{
				AWordGame* WordGame = Cast<AWordGame>(InvadedGM->GetCurrentFightGame());
				if (WordGame)
				{	
					SequenceArray.Push(WordGame->GetNextSequence(InvadedData));
				}
			}
			else
			{
				CurrentSequence = "Fail2";
			}
	}

	
}

void UFightGameComponent::SentenceSucceed(int32 Length)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		SentenceSucceedServer(Length);
	}
	else
	{	
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			if (InvadedGM->GetCurrentFightGame())
			{
				InvadedGM->GetCurrentFightGame()->UpdateScore(InvadedData, Length);
			}
		}	
	}
	
}

void UFightGameComponent::SentenceFail()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		SentenceFailServer();
	}
	else
	{
		AInvadedGM* InvadedGM =Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AWordGame* WordGame = Cast<AWordGame>(InvadedGM->GetCurrentFightGame());
			if (WordGame)
			{
				WordGame->UpdateScore(InvadedData, -1);
				WordGameHealth--;
			}
		}
	}
}

void UFightGameComponent::TimeIsUp()
{
	if (!(GetOwnerRole() < ROLE_Authority))
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			if (InvadedGM->GetCurrentFightGame())
			{
				InvadedGM->GetCurrentFightGame()->DetermineFightWinner();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TimeIsUp fail"));
			}
		}
	}
}


void UFightGameComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	DOREPLIFETIME(UFightGameComponent, CurrentSequence);
	DOREPLIFETIME(UFightGameComponent, GameLength);
	DOREPLIFETIME(UFightGameComponent, SequenceArray);
	DOREPLIFETIME(UFightGameComponent, TargetArray);
	DOREPLIFETIME(UFightGameComponent, WordGameHealth);
	DOREPLIFETIME(UFightGameComponent, ClickGameHealth);
	DOREPLIFETIME(UFightGameComponent, PlayerChar);
	DOREPLIFETIME(UFightGameComponent, InvadedData);
	DOREPLIFETIME(UFightGameComponent, EnemyRemainingTries);
	DOREPLIFETIME(UFightGameComponent, EnemyName);
	DOREPLIFETIME(UFightGameComponent, EnemyScore);
	DOREPLIFETIME(UFightGameComponent, Enemy);
	DOREPLIFETIME(UFightGameComponent, Score);
}
void UFightGameComponent::UpdateEnemyServer_Implementation()
{
	UpdateEnemy();
}
bool UFightGameComponent::UpdateEnemyServer_Validate()
{
	return true;
}
void UFightGameComponent::UpdateEnemyRemainingTriesServer_Implementation()
{
	UpdateEnemyRemainingTries();
}
bool UFightGameComponent::UpdateEnemyRemainingTriesServer_Validate()
{
	return true;
}

void UFightGameComponent::UpdateEnemyNameServer_Implementation()
{
	UpdateEnemyName();
}
bool UFightGameComponent::UpdateEnemyNameServer_Validate()
{
	return true;
}

void UFightGameComponent::UpdateEnemyScoreServer_Implementation()
{
	UpdateEnemyScore();
}
bool UFightGameComponent::UpdateEnemyScoreServer_Validate()
{
	return true;
}

void UFightGameComponent::UpdatePlayerScoreServer_Implementation()
{
	UpdatePlayerScore();
}
bool UFightGameComponent::UpdatePlayerScoreServer_Validate()
{
	return true;
}