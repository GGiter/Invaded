// Fill out your copyright notice in the Description page of Project Settings.

#include "CellBase.h"
#include "Components/StaticMeshComponent.h"
#include "InvadedPC.h"
#include "InvadedPlayer.h"
#include "InvadedCharacter.h"
#include "UnrealNetwork.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "InvadedMovementComp.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
ACellBase::ACellBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	S_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("S_Mesh"));
	bFocusable = true;
	bWalkable = true;
	UStaticMesh* MeshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Core/Cube")));

	if (MeshToUse && S_Mesh)
	{
		S_Mesh->SetStaticMesh(MeshToUse);
	}

	S_Mesh->MarkRenderStateDirty();
	RootComponent = S_Mesh;
	bIsOccupied = false;
	OccupyingPlayer = nullptr;
}

float ACellBase::CalculateRotation(AActor * Caller)
{

	AInvadedCharacter* InvadedPlayer = Cast<AInvadedCharacter>(Caller);


	ACellBase* OversteppedCell = nullptr;
	UInvadedMovementComp* InvadedMovementComp = nullptr;
	if (InvadedPlayer)
	{
		OversteppedCell = InvadedPlayer->GetOversteppedCell();
		InvadedMovementComp = InvadedPlayer->GetInvadedMovementComp();
	}

	FVector VDist = GetActorLocation() - OversteppedCell->GetActorLocation();
	VDist.Normalize();
	FVector ForwardVector = InvadedPlayer->GetCapsuleComponent()->GetForwardVector();

	float Rotation;
	if (FMath::IsNearlyEqual(VDist.X, ForwardVector.X, 0.5f) && FMath::IsNearlyEqual(VDist.Y, ForwardVector.Y, 0.5f))
	{
		Rotation = 0.0f;
	}
	else if (FMath::IsNearlyEqual(VDist.X*-1.0f, ForwardVector.X, 0.5f) && FMath::IsNearlyEqual(VDist.Y*-1.0f, ForwardVector.Y, 0.5f))
	{
		Rotation = 180.0f;
	}
	else if (FMath::IsNearlyEqual(ForwardVector.X, 1.0f, 0.5f) && FMath::IsNearlyEqual(VDist.Y, 1.0f, 0.5f) || FMath::IsNearlyEqual(ForwardVector.X, -1.0f, 0.5f) && FMath::IsNearlyEqual(VDist.Y, -1.0f, 0.5f) || FMath::IsNearlyEqual(ForwardVector.Y, 1.0f, 0.5f) && FMath::IsNearlyEqual(VDist.X, -1.0f, 0.5f) || FMath::IsNearlyEqual(ForwardVector.Y, -1.0f, 0.5f) && FMath::IsNearlyEqual(VDist.X, 1.0f, 0.5f))
	{
		Rotation = 90.0f;
	}
	else
	{
		Rotation = -90.0f;
	}
	return Rotation;
}

void ACellBase::SetGCost(uint16 Cost)
{
	GCost = Cost;
}

void ACellBase::SetHCost(uint16 Cost)
{
	HCost = Cost;
}

void ACellBase::SetParent(ACellBase * Cell)
{
	Parent = Cell;
}

int16 ACellBase::CompareCells(ACellBase * Cell)
{
	if (GetFCost() == Cell->GetFCost())
	{
		if (HCost > Cell->GetHCost())
			return -1;
		else if (HCost < Cell->GetHCost())
			return 1;
		else
			return 0;
	}
	else
	{
		if (GetFCost() > Cell->GetFCost())
			return -1;
		else if (GetFCost() < Cell->GetFCost())
			return 1;
	}
	return 0;
}
void ACellBase::Activate(APawn * Caller)
{
	AInvadedCharacter* InvadedPlayer = Cast<AInvadedCharacter>(Caller);
	if (InvadedPlayer)
	{
		if (GetFlags().Num())
		{
			InvadedPlayer->AddFlags(GetFlags());
			RemoveFlags();
		}
	}

}
void ACellBase::OnInteract_Implementation(APawn * Caller)
{
	AInvadedPC* InvadedPC = Cast<AInvadedPC>(Caller->GetController());
	AInvadedCharacter* InvadedPlayer = Cast<AInvadedCharacter>(Caller);


	ACellBase* OversteppedCell = nullptr;
	UInvadedMovementComp* InvadedMovementComp = nullptr;
	if (InvadedPlayer)
	{
		OversteppedCell = InvadedPlayer->GetOversteppedCell();
		InvadedMovementComp = InvadedPlayer->GetInvadedMovementComp();
	}
	if (!OversteppedCell)
	{
		InvadedPlayer->FindOversteppedCell();
		OversteppedCell = InvadedPlayer->GetOversteppedCell();
	}

	if (OversteppedCell&&InvadedMovementComp)
	{
		
			FVector Origin, Extent;
			GetActorBounds(false, Origin, Extent);
	
			FVector VDist = GetActorLocationTo(OversteppedCell) - OversteppedCell->GetActorLocationTo(this);
			VDist.Normalize();
			FVector ForwardVector = InvadedPlayer->GetCapsuleComponent()->GetForwardVector();
			
			float Rotation;
			if (FMath::IsNearlyEqual(VDist.X,ForwardVector.X,0.1f)&&FMath::IsNearlyEqual(VDist.Y,ForwardVector.Y,0.1f))
			{
				Rotation = 0.0f;
			}
			else if (FMath::IsNearlyEqual(VDist.X*-1.0f, ForwardVector.X, 0.1f) && FMath::IsNearlyEqual(VDist.Y*-1.0f, ForwardVector.Y, 0.1f))
			{
				Rotation = 180.0f;
			}
			else if (FMath::IsNearlyEqual(ForwardVector.X, 1.0f, 0.1f)&&FMath::IsNearlyEqual(VDist.Y, 1.0f, 0.1f)|| FMath::IsNearlyEqual(ForwardVector.X, -1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.Y, -1.0f, 0.1f)||FMath::IsNearlyEqual(ForwardVector.Y, 1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.X, -1.0f, 0.1f)||FMath::IsNearlyEqual(ForwardVector.Y, -1.0f, 0.1f) && FMath::IsNearlyEqual(VDist.X, 1.0f, 0.1f))
			{
				Rotation = 90.0f;
			}
			else
			{
				Rotation = -90.0f;
			}


			InvadedMovementComp->SetDestination(Origin);

			InvadedMovementComp->SetTurnOnly(false);


			InvadedMovementComp->Turn(Rotation);

		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement Componend cast fail"));
	}
}

void ACellBase::SetHighlightState_Implementation(EHighlightState Value)
{
	
	if (bWalkable&&bFocusable&&Value!=EHighlightState::HC_Inactive)
	{
		switch (Value)
		{
		case EHighlightState::HC_Negative:
		{
			DynamicTileMaterial->SetVectorParameterValue("TileGlow", NegativeGlowColor);
			break;
		}
		case EHighlightState::HC_Special:
		{
			DynamicTileMaterial->SetVectorParameterValue("TileGlow", SpecialGlowColor);
			break;
		}
		case EHighlightState::HC_Positive:
		{
			DynamicTileMaterial->SetVectorParameterValue("TileGlow", PositiveGlowColor);
			break;
		}
		case EHighlightState::HC_Inactive:
		{
			DynamicTileMaterial->SetVectorParameterValue("TileGlow", InactiveColor);
		}
		}
	}
	else if(Value== EHighlightState::HC_Inactive||!bWalkable)
	{
		DynamicTileMaterial->SetVectorParameterValue("BaseColor", InactiveColor);
		DynamicTileMaterial->SetVectorParameterValue("TileGlow", InactiveColor);
		DynamicTileMaterial->SetScalarParameterValue("TileGlowOn", 0.0f);

		BaseColor = InactiveColor;

		S_Mesh->SetMaterial(0, DynamicTileMaterial);
	}

	
}

void ACellBase::OnBeginFocus_Implementation()
{
	if(S_Mesh&&DynamicTileMaterial&&bWalkable&&bFocusable)
		DynamicTileMaterial->SetScalarParameterValue("TileGlowOn", 1.0f);

	
}

void ACellBase::OnEndFocus_Implementation()
{
	if (S_Mesh&&DynamicTileMaterial&&bWalkable&&bFocusable)
		DynamicTileMaterial->SetScalarParameterValue("TileGlowOn", 0.0f);
}

void ACellBase::SetBaseColor(const FLinearColor & Value)
{
	BaseColor = Value;

	ForceNetUpdate();
}

void ACellBase::SetIsWalkable(bool Value)
{
	bWalkable = Value;

	if (Value==false)
	{
		if (GetOccupyingPlayer())
		{
			UE_LOG(LogTemp, Warning, TEXT("ChangePlayerLocation"));

			AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOccupyingPlayer());
			if (InvadedCharacter)
			{
				FVector Origin, Extent;
				GetActorBounds(false, Origin, Extent);

				if (InvadedCharacter->GetStartingCell()->GetOccupyingPlayer() == nullptr)
				{
					InvadedCharacter->SetActorLocation(InvadedCharacter->GetStartingCell()->GetActorLocation());

					InvadedCharacter->TeleportPlayer(InvadedCharacter, InvadedCharacter->GetStartingCell()->GetActorLocation() + FVector(0.0f, 0.0f, 100.f));
					InvadedCharacter->SetOversteppedCell(InvadedCharacter->GetStartingCell());
				}
				else
				{
					InvadedCharacter = Cast<AInvadedCharacter>(InvadedCharacter->GetStartingCell()->GetOccupyingPlayer());

					InvadedCharacter->SetActorLocation(InvadedCharacter->GetStartingCell()->GetActorLocation());

					InvadedCharacter->TeleportPlayer(InvadedCharacter, InvadedCharacter->GetStartingCell()->GetActorLocation() + FVector(0.0f, 0.0f, 100.f));
					InvadedCharacter->SetOversteppedCell(InvadedCharacter->GetStartingCell());

					InvadedCharacter = Cast<AInvadedCharacter>(GetOccupyingPlayer());

					InvadedCharacter->SetActorLocation(InvadedCharacter->GetStartingCell()->GetActorLocation());

					InvadedCharacter->TeleportPlayer(InvadedCharacter, InvadedCharacter->GetStartingCell()->GetActorLocation() + FVector(0.0f, 0.0f, 100.f));
					InvadedCharacter->SetOversteppedCell(InvadedCharacter->GetStartingCell());
				}

				
			}
		}


		/*Disabling color material*/

		DynamicTileMaterial->SetVectorParameterValue("BaseColor", InactiveColor);
		DynamicTileMaterial->SetVectorParameterValue("TileGlow", InactiveColor);
		DynamicTileMaterial->SetScalarParameterValue("TileGlowOn", 0.0f);

		S_Mesh->SetMaterial(0, DynamicTileMaterial);

		BaseColor = InactiveColor;
	}
}

void ACellBase::SetIsFocusable(bool Value)
{
	bFocusable = Value;
}
FVector ACellBase::GetActorLocationTo(AActor * Actor)
{
	return GetActorLocation();
}
void ACellBase::BeginPlay()
{
	Super::BeginPlay();
	if (TileMaterial)
	{
		DynamicTileMaterial = UMaterialInstanceDynamic::Create(TileMaterial, this);
		S_Mesh->SetMaterial(0, DynamicTileMaterial);
		SetHighlightState(EHighlightState::HC_Positive);
	}
}
void ACellBase::AddFlag(AFlag * Flag)
{
	if (Flags.Find(Flag) == INDEX_NONE)
		Flags.Add(Flag);
}
void ACellBase::RemoveFlag(AFlag * Flag)
{
	if (Flags.Find(Flag) != INDEX_NONE)
		Flags.Remove(Flag);
}
void ACellBase::RemoveFlags()
{
	Flags.Empty();
}
void ACellBase::UpdateCellMaterial(const FLinearColor& BaseColorValue, EHighlightState HighlightState)
{
	DynamicTileMaterial->SetVectorParameterValue("BaseColor", BaseColorValue);

	SetHighlightState(HighlightState);

	S_Mesh->SetMaterial(0, DynamicTileMaterial);
}
void ACellBase::UpdateCellState()
{
	if (!bWalkable)
	{
		SetHighlightState(EHighlightState::HC_Inactive);

	//	DynamicTileMaterial->SetVectorParameterValue("TileColor", InactiveColor);
	}
}
void ACellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DynamicTileMaterial)
	{
		DynamicTileMaterial->SetVectorParameterValue("BaseColor", BaseColor);
		if (!bWalkable)
		{
			DynamicTileMaterial->SetScalarParameterValue("TileGlowOn", 0.0f);
		}
		
	}
		
}
void ACellBase::SetCellType_Implementation(ECellType Value)
{
	CellType = Value;
}
bool ACellBase::SetCellType_Validate(ECellType Value)
{
	return true;
}
void ACellBase::SetOccupyingPlayer(APawn* Player)
{
	OccupyingPlayer = Player;
}
void ACellBase::SetIsOccupied(bool Value)
{
	bIsOccupied = Value;
}
void ACellBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACellBase, Flags);
	DOREPLIFETIME(ACellBase, bWalkable);
	DOREPLIFETIME(ACellBase, CellType);
	DOREPLIFETIME(ACellBase, bFocusable);
	DOREPLIFETIME(ACellBase, bIsOccupied);
	DOREPLIFETIME(ACellBase, OccupyingPlayer);
	DOREPLIFETIME(ACellBase, BaseColor);
}
