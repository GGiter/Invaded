// Fill out your copyright notice in the Description page of Project Settings.

#include "AICardManager.h"
#include "AI/InvadedAIPlayer.h"
#include "Alg/FAStarNT.h"
#include "Kismet/GameplayStatics.h"
#include "Cards/Attack/CardDoubleTime.h"
#include "Cards/Attack/CardAirAssault.h"
#include "Cards/Attack/CardAmbush.h"
#include "Cards/Attack/CardCloseAirSupport.h"
#include "Cards/Attack/CardNavalStrike.h"
#include "Cards/Defend/CardEspionage.h"
#include "Cards/Defend/CardFortify.h"
#include "Cards/Defend/CardGroundSupport.h"
#include "Cards/Defend/CardNuke.h"
#include "Cards/Defend/CardSpecOps.h"


// Sets default values for this component's properties
UAICardManager::UAICardManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UAICardManager::ChooseCards()
{
	
	
	
	if(PlayerRef->bUseCards)
	{
		if (HasCard(ACardDoubleTime::StaticClass()))
		{
			if (PlayerRef->GetDesiredCell())
			{
				
					TaskStack.Push(ACardDoubleTime::StaticClass());
				
			}
			else if (PlayerRef->GetDesiredPlayer())
			{
				if (GetDistance(PlayerRef->GetOversteppedCell(), PlayerRef->GetDesiredPlayer()->GetOversteppedCell()) < 4)
				{
					TaskStack.Push(ACardDoubleTime::StaticClass());
				}
			}
			else if (FMath::Rand() > 0.5f||PlayerRef->DoesAnyoneHaveYourFlag())
			{
				TaskStack.Push(ACardDoubleTime::StaticClass());
			}
		}
		if (HasCard(ACardFortify::StaticClass()))
		{
			if (PlayerRef->GetDesiredCell())
			{
				
					TaskStack.Push(ACardFortify::StaticClass());
				
			}
			else if (PlayerRef->GetDesiredPlayer())
			{
				if (GetDistance(PlayerRef->GetOversteppedCell(), PlayerRef->GetDesiredPlayer()->GetOversteppedCell()) < 4)
				{
					TaskStack.Push(ACardFortify::StaticClass());
				}
			}
			else if (FMath::Rand() > 0.5f || PlayerRef->DoesAnyoneHaveYourFlag())
			{
				TaskStack.Push(ACardFortify::StaticClass());
			}
		}
		if (HasCard(ACardAirAssault::StaticClass()))
		{
			if (FMath::Rand() > 0.75f)
			{
				TaskStack.Push(ACardAirAssault::StaticClass());
			}	
		}
		if (HasCard(ACardCloseAirSupport::StaticClass()))
		{
			if (PlayerRef->DoesAnyoneHaveYourFlag())
			{
				if(GetDistance(PlayerRef->GetOversteppedCell(), PlayerRef->DoesAnyoneHaveYourFlag()->GetOversteppedCell()) < 3)
				{
					TaskStack.Push(ACardCloseAirSupport::StaticClass());
				}
			}	
		}
		if (HasCard(ACardAmbush::StaticClass()))
		{
			if (PlayerRef->DoesAnyoneHaveYourFlag()||IsAnyoneInDistance(5,PlayerRef->GetYourFlag()->GetLastOversteppedCell()))
			{
				PlayerRef->SetDesiredPlayer(IsAnyoneInDistance(5, PlayerRef->GetYourFlag()->GetLastOversteppedCell()));
				TaskStack.Push(ACardAmbush::StaticClass());
			}
		}
		if (HasCard(ACardNuke::StaticClass()))
		{
			if (PlayerRef->GetFlags().Num())
			{
				TaskStack.Push(ACardNuke::StaticClass());
			}
			
		}
		if (HasCard(ACardNavalStrike::StaticClass()))
		{
			if (PlayerRef->DoesAnyoneHaveYourFlag()&& IsAnyoneInDistance(5, PlayerRef->GetYourFlag()->GetCornerCell()))
			{
				TaskStack.Push(ACardNavalStrike::StaticClass());
			}
		
		}
		if (HasCard(ACardEspionage::StaticClass()))
		{
			if (PlayerRef->GetDesiredPlayer())
			{
				if (HasCard(ACardCloseAirSupport::StaticClass()) && TaskStack.Find(ACardCloseAirSupport::StaticClass()) == INDEX_NONE)
				{
					TaskStack.Push(ACardCloseAirSupport::StaticClass());
				}
				if (TaskStack.Find(ACardCloseAirSupport::StaticClass()) != INDEX_NONE)
				{
					if (GetDistance(PlayerRef->GetOversteppedCell(), PlayerRef->GetDesiredPlayer()->GetOversteppedCell()) < 2)
					{
						TaskStack.Push(ACardEspionage::StaticClass());
					}
				}
			}
			
		}
		if (HasCard(ACardGroundSupport::StaticClass()))
		{
			if (PlayerRef->GetDesiredPlayer())
			{
				if (GetDistance(PlayerRef->GetOversteppedCell(), PlayerRef->GetDesiredPlayer()->GetOversteppedCell()) < 4)
				{
					TaskStack.Push(ACardGroundSupport::StaticClass());
				}
			}
		}
	}
	
	NextTask();
}
void UAICardManager::NextTask()
{
	PlayerRef = Cast<AInvadedAIPlayer>(GetOwner());
	/*
	if (TaskStack.Num())
	{
		if (PlayerRef->GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_Base&&PlayerRef->GetInvadedPS()->GetEnemyAction() == EEnemyAction::EA_Base)
		{
			UseCardClass(TaskStack.Pop());

			GetWorld()->GetTimerManager().SetTimer(NextTaskHandle, this, &UAICardManager::NextTask, 3.0f, false);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(NextTaskHandle, this, &UAICardManager::NextTask, 3.0f, false);

			PlayerRef->ChoosePlayerMode();
		}
	}
	else
	{
		PlayerRef->ThrowDice();

		PlayerRef->ChoosePlayerMode();
	
	}
	*/
	PlayerRef->ThrowDice();

	PlayerRef->ChoosePlayerMode();
	
}
int32 UAICardManager::GetDistance(ACellBase * A, ACellBase * B)
{
	if (GridGenerator)
		return UFAStarNT::GetPath(A, B, GridGenerator).Num();
	else
		return 1000;
}
// Called when the game starts
void UAICardManager::BeginPlay()
{
	Super::BeginPlay();
	PlayerRef = Cast<AInvadedAIPlayer>(GetOwner());
	// ...	
}

bool UAICardManager::HasCard(UClass * CardClass)
{
	for (ACardBase* Card : PlayerRef->GetCardArray())
	{
		if (Card->IsA(CardClass))
		{
			return true;
		}
	}
	return false;
}

void UAICardManager::UseCardClass(UClass * CardClass)
{
	for (ACardBase* Card : PlayerRef->GetCardArray())
	{
		if (Card->IsA(CardClass))
		{
			PlayerRef->UseCard(Card);
			break;
		}
	}
}


int32 UAICardManager::GetClosestEnemyDist()
{
	return int32();
}

AInvadedCharacter* UAICardManager::IsAnyoneInDistance(int32 Distance, ACellBase * Cell)
{
	if (Cell)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), OutActors);
		for (AActor* Actor : OutActors)
		{
			AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(Actor);
			if (InvadedCharacter&&InvadedCharacter != PlayerRef)
			{
				if (GetDistance(InvadedCharacter->GetOversteppedCell(), Cell) <= Distance)
				{
					return InvadedCharacter;
				}
			}
		}
	}
	return nullptr;
}

// Called every frame
void UAICardManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

