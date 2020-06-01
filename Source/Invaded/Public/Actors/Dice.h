// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dice.generated.h"

UCLASS()
class INVADED_API ADice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADice();

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	uint8 CalculateValue(FName Socket);

	void SetFirstColor(FLinearColor Value);

	void SetSecondColor(FLinearColor Value);

	UStaticMeshComponent* GetMesh() const { return S_Mesh; }
protected:

	UFUNCTION()
	void FindClosestSocket();

	void CheckLocation();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* S_Mesh;

private:

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		TArray<FName> SocketNames;//value = index + 1

	UPROPERTY(EditDefaultsOnly,meta=(AllowPrivateAccess="true"))
	UMaterialInterface* Material;

	UPROPERTY(Replicated)
	FLinearColor FirstColor;

	UPROPERTY(Replicated)
	FLinearColor SecondColor;

	FVector LastLocation;
	FTimerHandle LocationTimerHandle;
	
};
