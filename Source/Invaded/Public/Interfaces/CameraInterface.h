// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Interface.h"
#include "CameraInterface.generated.h"

UINTERFACE(BlueprintType)
class INVADED_API UCameraInterface : public UInterface
{
	GENERATED_BODY()
};

class INVADED_API ICameraInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Zoom(const float& Amount);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void MoveForward(const float& Amount);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void MoveRight(const float& Amount);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Rotate(const float& Amount);



};