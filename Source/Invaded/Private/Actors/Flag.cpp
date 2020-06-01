// Fill out your copyright notice in the Description page of Project Settings.

#include "Flag.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"
#include "InvadedPlayer.h"
#include "InvadedCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "InvadedStateMachine.h"
#include "TimerManager.h"
#include "Actors/CellBase.h"


// Sets default values
AFlag::AFlag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = BoxComponent;

	SK_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("SK_Mesh");
	SK_Mesh->SetupAttachment(BoxComponent);
	bReplicates = true;

}

// Called when the game starts or when spawned
void AFlag::BeginPlay()
{
	Super::BeginPlay();
	
	if (Material)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
		SK_Mesh->SetMaterial(0, DynMaterial);
	}
	
}

// Called every frame
void AFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DynMaterial)
	{
		DynMaterial->SetVectorParameterValue("P_Color1", FirstColor);
		DynMaterial->SetVectorParameterValue("P_Color2", SecondColor);
	}
		
}
void AFlag::SetOwnerName(FName Value)
{
	OwnerName = Value;
}
void AFlag::DestroyFlag_Implementation(AInvadedCharacter* Player)
{
	Destroy();
}
bool AFlag::DestroyFlag_Validate(AInvadedCharacter* Player)
{
	return true;
}
void AFlag::SetFlagTextureIndex(const int32& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Flag Rep %d"), FlagTextureIndex);
	if (Value >= 0 && Value < FlagTextureArray.Num() && Value < FlagTextureArray2.Num())
	{

		FlagTextureIndex = Value;
		DoRep_FlagTextureIndex = !DoRep_FlagTextureIndex;
		
		OnRep_FlagTextureIndex();
	}
}
void AFlag::SetFirstColor(FLinearColor Value)
{
	FirstColor = Value;
	
	UpdateMaterial();
}

void AFlag::SetSecondColor(FLinearColor Value)
{
	SecondColor = Value;

	UpdateMaterial();
}
void AFlag::UpdateMaterial_Implementation()
{
	if (DynMaterial)
	{
		DynMaterial->SetVectorParameterValue("P_Color1", FirstColor);
		DynMaterial->SetVectorParameterValue("P_Color2", SecondColor);
	}
}
bool AFlag::UpdateMaterial_Validate()
{
	return true;
}
void AFlag::RemoveFlagTick()
{
	if (CurrentOwner)
	{
		if (CurrentOwner->GetOversteppedCell() != LastOversteppedCell)
		{
			CurrentOwner = nullptr;
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RemoveFlagHandle);
	}
}
void AFlag::RemoveFromPlayer()
{
	if (Role < ROLE_Authority)
	{
		RemoveFromPlayerServer();
	}
	else
	{
		if (CurrentOwner)
		{
			CurrentOwner->RemoveFlag(this);
			SetLastOversteppedCell(CurrentOwner->GetOversteppedCell());
			DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepRelative,EDetachmentRule::KeepRelative,EDetachmentRule::KeepWorld,true));
			CurrentOwner = nullptr;
		}
	}
}
void AFlag::SetDeathCell(ACellBase * Value)
{
	DeathCell = Value;
}
void AFlag::SetCornerCell(ACellBase * Value)
{
	CornerCell = Value;

	if (Value)
	{
		Value->SetBaseColor(SecondColor);
	}
}
void AFlag::SetCurrentOwner(AInvadedCharacter * Value)
{
	CurrentOwner = Value;
}
void AFlag::SetFirstOwner(AInvadedCharacter * Value)
{
	FirstOwner = Value;
}
void AFlag::FinishSetup()
{
	if (CornerCell)
	{
		CornerCell->SetBaseColor(SecondColor);

		AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (InvadedCharacter)
		{
			InvadedCharacter->UpdateCellMaterial(CornerCell,SecondColor, EHighlightState::HC_Positive);
		}
	}
}
void AFlag::SetLastOversteppedCell(ACellBase * Value)
{
	if (LastOversteppedCell)
	{
		LastOversteppedCell->RemoveFlag(this);
	}

	LastOversteppedCell = Value;

	if (LastOversteppedCell)
	{
		LastOversteppedCell->AddFlag(this);
	}
}
void AFlag::OnRep_FlagTextureIndex()
{
	UE_LOG(LogTemp, Warning, TEXT("Flag Rep %d"), FlagTextureIndex);
	DynMaterial->SetTextureParameterValue("ML_Mask", FlagTextureArray[FlagTextureIndex]);
	DynMaterial->SetTextureParameterValue("Symbol", FlagTextureArray2[FlagTextureIndex]);
	DynMaterial->SetTextureParameterValue("SymbolMask", FlagTextureArray[FlagTextureIndex]);
}
void AFlag::RemoveFromPlayerServer_Implementation()
{
	RemoveFromPlayer();
}
bool AFlag::RemoveFromPlayerServer_Validate()
{
	return true;
}
void AFlag::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlag, OwnerName);
	DOREPLIFETIME(AFlag, FirstColor);
	DOREPLIFETIME(AFlag, SecondColor);
	DOREPLIFETIME(AFlag, CurrentOwner);
	DOREPLIFETIME(AFlag, LastOversteppedCell);
	DOREPLIFETIME(AFlag, DeathCell);
	DOREPLIFETIME(AFlag, CornerCell);
	DOREPLIFETIME(AFlag, FirstOwner);
	DOREPLIFETIME(AFlag, FlagTextureIndex);
	DOREPLIFETIME(AFlag, DoRep_FlagTextureIndex);
}