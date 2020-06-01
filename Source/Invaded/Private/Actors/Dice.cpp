// Fill out your copyright notice in the Description page of Project Settings.

#include "Dice.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameModes/InvadedGM.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "Engine/World.h"
#include "DicePod.h"
#include "UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ADice::ADice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	S_Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	S_Mesh->SetupAttachment(RootComponent);
	RootComponent = S_Mesh;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(S_Mesh);
	
	bReplicates = true;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void ADice::BeginPlay()
{
	Super::BeginPlay();
	LastLocation = GetActorLocation();
	GetWorldTimerManager().SetTimer(LocationTimerHandle, this, &ADice::CheckLocation, 0.5f, true);
}
void ADice::CheckLocation()
{
	if (LastLocation == GetActorLocation())
	{
		FindClosestSocket();
		GetWorldTimerManager().ClearTimer(LocationTimerHandle);
	}
	else
	{
		LastLocation = GetActorLocation();
	}
}
void ADice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);

	DynMaterial->SetVectorParameterValue("P_Color1", FirstColor);
	DynMaterial->SetVectorParameterValue("P_Color2", SecondColor);
	S_Mesh->SetMaterial(0, DynMaterial);
}

void ADice::FindClosestSocket()
{
	float Highest = MAX_FLT;
	FName SocketShowing;
	for (FName Socket : S_Mesh->GetAllSocketNames())
	{
		if (S_Mesh->GetSocketLocation(Socket).Z < Highest)
		{
			Highest = S_Mesh->GetSocketLocation(Socket).Z;
			SocketShowing = Socket;
		}
			
	}
	S_Mesh->SetSimulatePhysics(false);
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (InvadedGM)
	{
		InvadedGM->DiceHaveLanded(this, CalculateValue(SocketShowing));
		InvadedGM->GetDicePod()->SetDiceValue(CalculateValue(SocketShowing));
	}
	
}

uint8 ADice::CalculateValue(FName Socket)
{	
	for (int8 i = 0; i < SocketNames.Num(); ++i)
	{
		if (SocketNames[i] == Socket)
			return i + 1;
	}
	return 0;
}
void ADice::SetFirstColor(FLinearColor Value)
{
	FirstColor = Value;
}
void ADice::SetSecondColor(FLinearColor Value)
{
	SecondColor = Value;
}
void ADice::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADice, FirstColor);
	DOREPLIFETIME(ADice, SecondColor);
}
