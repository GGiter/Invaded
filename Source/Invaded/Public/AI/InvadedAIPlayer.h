// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InvadedCharacter.h"
#include "TimerManager.h"
#include "InvadedAIPlayer.generated.h"

UCLASS()
class INVADED_API AInvadedAIPlayer : public AInvadedCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAcess = "true"))
		class UAICardManager* AICardManager;
public:
	// Sets default values for this character's properties
	AInvadedAIPlayer(const FObjectInitializer& ObjectInitializer);

	void SetupPlayer(FString PlayerName,int32 Number);

	void NextTurn(class UInvadedData* CurrentPS);

	virtual void CallTurnEnd() override;
//Fight Interface
	virtual void EnterFightMode(class AActor* Enemy) override;

	virtual void FightWon_Implementation(class AInvadedCharacter* Enemy) override;

	virtual void FightLost_Implementation(class AInvadedCharacter* Enemy) override;

	virtual void FightDraw_Implementation(class AInvadedCharacter* Enemy) override;

	virtual void GameWon() override;

	virtual void ToggleDicePod() override;
	
	virtual void KilledPlayer() override;

	virtual void GameLost() override;

	void PlayTurn();

	

	void FinishFightMode();

	uint32 MinimalMoves;

	class ACellBase* GetDesiredCell() { return DesiredCell; }

	AInvadedCharacter* GetDesiredPlayer() { return DesiredPlayer; }

	void SetDesiredPlayer(AInvadedCharacter* Value);

	AInvadedCharacter* DoesAnyoneHaveYourFlag();

	AFlag* GetYourFlag() { return YourFlag; }

	void ChoosePlayerMode();

	AActor* FindClosestActor(UClass* ActorClass);

	UPROPERTY(EditAnywhere, Category = "Player")
		bool bUseCards;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void FindPath(class ACellBase* StartCell, class ACellBase* EndCell) override;
	float DetermineValue(class ACellBase * Cell);
	float DeterminePlayerValue(class AInvadedCharacter* Player);
	void ChooseEnemyAction();

	TArray<class ACellBase*> CalculatePath(AInvadedCharacter* Player, int32 Distance);
	
	void SelectCornerCell();
	void SelectPlayer();

	bool DoesPlayerHaveYourFlag(class AInvadedCharacter* Player);
	AFlag* DoYouHaveEnemyFlag();
	
	AFlag* FindYourFlag();

	void DetermineRatios();
	int32 FindDistanceTo(class ACellBase* StartCell,AActor* Actor);

	

	UPROPERTY(EditAnywhere, Category = "Ratios")
		float PlayerRatio;
	UPROPERTY(EditAnywhere, Category = "Ratios")
		float FlagRatio;
	UPROPERTY(EditAnywhere, Category = "Ratios")
		float AttackCellRatio;
	UPROPERTY(EditAnywhere, Category = "Ratios")
		float DefenseCellRatio;

		float CustomCellRatio;

		bool bAggressive;

	class ACellBase* DesiredCell;

	class AInvadedCharacter* SelectedPlayer;

	class ACellCorner* SelectedCornerCell;

	class AInvadedCharacter* DesiredPlayer;

	class AFlag* YourFlag;

	UPROPERTY(EditAnywhere, Category = "Player")
		bool bAttackHumanPlayer;
	UPROPERTY(EditAnywhere, Category = "Player")
		bool bAttackPlayer;
	UPROPERTY(EditAnywhere, Category = "Player")
		bool bAvoidPlayer;
	UPROPERTY(EditAnywhere, Category = "Player")
		bool bSkipTurn;
	UPROPERTY(EditAnywhere, Category = "Player")
		TArray<FLinearColor> ColorPool;

	bool bDicePodReady;
	
	bool bHasFoundPath = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bIsCurrentlyWorking;

	FTimerDelegate HighlightDelegate;

	FTimerHandle FindPathHandle;
	
	
};
