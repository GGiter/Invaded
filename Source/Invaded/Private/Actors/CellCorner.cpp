// Fill out your copyright notice in the Description page of Project Settings.

#include "CellCorner.h"
#include "UnrealNetwork.h"


void ACellCorner::AddHiddenCell_Implementation(ACellBase* Cell)
{
	HiddenCells.Add(Cell);
}
bool ACellCorner::AddHiddenCell_Validate(ACellBase* Cell)
{
	return true;
}
void ACellCorner::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACellCorner, HiddenCells);
}

FVector ACellCorner::GetActorLocationTo(AActor * Actor)
{
	int32 Index = 0;
	float Min = MAX_FLT;
	for (int32 i=0;i<HiddenCells.Num();++i)
	{
		float Dist=FVector::Distance(HiddenCells[i]->GetActorLocation(), Actor->GetActorLocation());
		if (Dist < Min)
		{
			Min = Dist;
			Index = i;
		}
	}
	return HiddenCells[Index]->GetActorLocation();
}
