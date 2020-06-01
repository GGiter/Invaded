// Fill out your copyright notice in the Description page of Project Settings.


#include "BugReporter.h"
#include "Player/PlayerHUD.h"

const FString TrelloCards = "https://api.trello.com/1/cards?";
const FString KeyTrello = "69c75f5e931a370c363f4af9bdb294ee";
const FString Token = "d810e6903c89d77729cb02f90b66c4b003e3e6f61ae945473e3de6d2e1576261";
const FString Label = "5cc72d7b91d0c2ddc5632cb5";
const FString List = "5cc72eff12841d76a5303fed";


void UBugReporter::ReportBug(FString Name, FString Description, class APlayerHUD* SenderHUDValue)
{
	this->SenderHUD = SenderHUDValue;
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("POST");

	//Replace all the spaces with +
	Name = Name.Replace(TEXT(" "), TEXT("+"));
	Description = Description.Replace(TEXT(" "), TEXT("+"));

	//Construct the http url
	FString URL = TrelloCards +
		"key=" + KeyTrello +
		"&token=" + Token +
		"&name=" + Name +
		"&desc=" + Description +
		"&idList=" + List +
		"&idLabels=" + Label;

	Request->SetURL(URL);
	//Call OnReportBugComplete once the request is done
	Request->OnProcessRequestComplete().BindUObject(this, &UBugReporter::OnReportBugComplete);
	//Do the request
	Request->ProcessRequest();
}

void UBugReporter::OnReportBugComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (SenderHUD)
	{
		SenderHUD->ReportBugComplete(bWasSuccessful);
	}
}
