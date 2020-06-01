// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AICardManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVADED_API UAICardManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAICardManager();
	void ChooseCards();

	void NextTask();

	int32 GetDistance(class ACellBase* A, class ACellBase* B);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	bool HasCard(UClass* CardClass);
	void UseCardClass(UClass* CardClass);

	class AInvadedAIPlayer* PlayerRef;

	TArray<UClass*> TaskStack;

	class AGridGenerator* GridGenerator;

	int32 GetClosestEnemyDist();

	class AInvadedCharacter* IsAnyoneInDistance(int32 Distance, class ACellBase* Cell);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	FTimerHandle NextTaskHandle;
};
