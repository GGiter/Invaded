// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FightGameComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVADED_API UFightGameComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFightGameComponent();

	UFUNCTION(BlueprintPure)
	FString GetCurrentSequence()  { 
		if (SequenceArray.Num())
		{
			FString RetVal = SequenceArray[0];
			SequenceArray.RemoveAt(0);
			if(RetVal=="")
				return FString("EmptySequence");
			else
			    return RetVal;
		}	
		else
		{
			return FString("EmptySequence");
		}
			
	}
	UFUNCTION(BlueprintPure)
		int32 GetCurrentTarget() {
		if (TargetArray.Num())
		{
			int32 RetVal = TargetArray[0];
			TargetArray.RemoveAt(0);
			return RetVal;
		}
		else
		{
			return 0;
		}

	}
	
	UFUNCTION(BlueprintPure)
		FString GetScore() const { return Score; }

	UFUNCTION(BlueprintPure)
		FString GetEnemyRemainingTries() const { return EnemyRemainingTries; }
	UFUNCTION(BlueprintPure)
		FString GetEnemyName() const { return EnemyName; }
	UFUNCTION(BlueprintPure)
		FString GetEnemyScore() const { return EnemyScore; }
	UFUNCTION(BlueprintPure)
		int32 GetClickGameHealth() const { return ClickGameHealth; }
	UFUNCTION(BlueprintPure)
		int32 GetWordGameHealth() const { return WordGameHealth; }
	UFUNCTION(BlueprintPure)
		int32 GetGameLength() const { return GameLength; }
	UFUNCTION(BlueprintPure)
		class UInvadedData*  GetEnemy() const { return Enemy; }
	void StartWordGame(int32 SentenceLength,int32 GameLengthValue,int32 WordGameHealthValue);

	void StartClickGame(int32 NumberOfStartTarget, int32 GameLenghtValue, int32 ClickGameHealthValue);

	UFUNCTION(BlueprintCallable)
		virtual void FightWon(class AInvadedCharacter* EnemyValue);
	UFUNCTION(BlueprintCallable)
		virtual void FightLost(class AInvadedCharacter* EnemyValue);
	UFUNCTION(BlueprintCallable)
		virtual void FightDraw(class AInvadedCharacter* EnemyValue);

	UFUNCTION(BlueprintCallable)
	void EndGameReached();

	UFUNCTION(BlueprintCallable)
	void UpdateScore(int32 Value);

	void UpdateData();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateEnemyRemainingTries();

	void UpdateEnemyName();

	void UpdateEnemyScore();

	void UpdatePlayerScore();

	void UpdateEnemy();

	UPROPERTY(Replicated)
	FString CurrentSequence;

	UPROPERTY(Replicated)
	TArray<FString> SequenceArray;
	UPROPERTY(Replicated)
	TArray<int32> TargetArray;

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateScoreServer(int32 Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void NextTargetServer();

	UFUNCTION(Server,Reliable,WithValidation)
		void NextSequenceServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void SentenceSucceedServer(int32 Length);

	UFUNCTION(Server, Reliable, WithValidation)
		void SentenceFailServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateEnemyRemainingTriesServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateEnemyNameServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateEnemyScoreServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdatePlayerScoreServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateEnemyServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void TimeIsUpServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void EndGameReachedServer();

	UPROPERTY(Replicated)
	class AInvadedCharacter* PlayerChar;
	UPROPERTY(Replicated)
	class UInvadedData* InvadedData;
	UPROPERTY(Replicated)
	int32 WordGameHealth;
	UPROPERTY(Replicated)
	int32 GameLength;
	UPROPERTY(Replicated)
	int32 ClickGameHealth;
	UPROPERTY(Replicated)
	FString EnemyRemainingTries;
	UPROPERTY(Replicated)
	FString EnemyName;
	UPROPERTY(Replicated)
	FString EnemyScore;
	UPROPERTY(Replicated)
	FString Score;
	UPROPERTY(Replicated)
	class UInvadedData* Enemy;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void NextTarget();
	UFUNCTION(BlueprintCallable)
		void NextSequence();
	UFUNCTION(BlueprintCallable)
		void SentenceSucceed(int32 Length);
	UFUNCTION(BlueprintCallable)
		void SentenceFail();
	UFUNCTION(BlueprintCallable)
		void TimeIsUp();
	
};
