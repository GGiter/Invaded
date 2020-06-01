// Fill out your copyright notice in the Description page of Project Settings.

#include "CardBase.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ACardBase::ACardBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ACardBase::OnInteract_Implementation(AInvadedCharacter* Caller)
{
}

void ACardBase::SetFirstColor(FLinearColor Value)
{
	MaterialDyn->SetVectorParameterValue("P_color1", Value);
}

void ACardBase::SetSecondColor(FLinearColor Value)
{
	MaterialDyn->SetVectorParameterValue("P_color2", Value);
}

// Called when the game starts or when spawned
void ACardBase::BeginPlay()
{
	Super::BeginPlay();
	if (MaterialRef)
	{
		MaterialDyn= UMaterialInstanceDynamic::Create(MaterialRef, this);
		if (MaterialDyn)
		{
			SetFirstColor(FirstColor);
			SetSecondColor(SecondColor);
		}
	}
	
}

// Called every frame
void ACardBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ACardBase::SpawnEffect(class AInvadedCharacter* Causer, class AInvadedCharacter* Target)
{
	if (Role < ROLE_Authority)
	{
		ServerSpawnEffect(Causer, Target);
	}
	else
	{
		MulticastSpawnEffect(Causer, Target);
	}
}
void ACardBase::Activate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target)
{
}
void ACardBase::ServerSpawnEffect_Implementation(class AInvadedCharacter* Causer, class AInvadedCharacter* Target)
{
	SpawnEffect(Causer, Target);
}
bool ACardBase::ServerSpawnEffect_Validate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target)
{
	return true;
}
void ACardBase::MulticastSpawnEffect_Implementation(class AInvadedCharacter* Causer, class AInvadedCharacter* Target)
{
	BP_SpawnEffect(Causer, Target);
}
bool ACardBase::MulticastSpawnEffect_Validate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target)
{
	return true;
}