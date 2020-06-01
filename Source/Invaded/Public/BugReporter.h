// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "BugReporter.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UBugReporter : public UObject
{
	GENERATED_BODY()
public:
	void ReportBug(FString Name, FString Description,class APlayerHUD* SenderHUDValue);

private:
	void OnReportBugComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	class APlayerHUD* SenderHUD;
};
