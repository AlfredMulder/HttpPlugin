// Copyright Epic Games, Inc. All Rights Reserved.

#include "HttpPluginBPLibrary.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UHttpPluginBPLibrary::UHttpPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

void UHttpPluginBPLibrary::Activate() {
	// Create HTTP Request
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().
		CreateRequest();
	const FRandomStream Cahracter(FMath::Rand());
	const auto Debug_URL = URL + FString::FromInt(Cahracter.RandRange(1, 671));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetURL(Debug_URL);
	// Setup Async response
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Request, const FHttpResponsePtr Response, const bool bSuccess) {
			FString ResponseString = "";
			const FString ImageURL = "";
			if (bSuccess) {
				ResponseString = Response->GetContentAsString();
			}

			this->HandleRequestCompleted(ResponseString, bSuccess, ImageURL);
		});

	// Handle actual request
	HttpRequest->ProcessRequest();
}

void UHttpPluginBPLibrary::ProcessingOutputString(FString& ResponseString,
                                                  const TSharedPtr<FJsonObject> JsonObject) {
	ResponseString = "";
	ResponseString += "id: " + JsonObject->GetStringField("id") + "\r\n";
	ResponseString += "name: " + JsonObject->GetStringField("name") + "\r\n";
	ResponseString += "status: " + JsonObject->GetStringField("status") + "\r\n";
	ResponseString += "species: " + JsonObject->GetStringField("species") + "\r\n";
	ResponseString += "type: " + JsonObject->GetStringField("type") + "\r\n";
	ResponseString += "gender: " + JsonObject->GetStringField("gender") + "\r\n";
	ResponseString += "origin: " + JsonObject->GetStringField("origin") + "\r\n";
	ResponseString += "location: " + JsonObject->GetStringField("location") + "\r\n";
	const auto EpisodeArray = JsonObject->GetArrayField("episode");

	FString EpisodeString;

	for (int Index = 0; Index < EpisodeArray.Num(); ++Index) {
		EpisodeString += FString::Printf(TEXT("%i %s \n"), Index, *EpisodeArray[Index]->AsString());
	}

	ResponseString += "episode: \r\n" + EpisodeString + "\r\n";
	ResponseString += "url: " + JsonObject->GetStringField("url") + "\r\n";
	ResponseString += "created: " + JsonObject->GetStringField("created") + "\r\n";
}

void UHttpPluginBPLibrary::HandleRequestCompleted(
	FString ResponseString,
	const bool bSuccess,
	FString ImageURL) const {
	if (bSuccess) {
		/* Deserialize object */
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<>::Create(
			ResponseString);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
			// Getting image URL to dowwload it later
			JsonObject->TryGetStringField("image", ImageURL);
			ProcessingOutputString(ResponseString, JsonObject);
		}
	}
	Completed.Broadcast(ResponseString, bSuccess, ImageURL);
}

UHttpPluginBPLibrary* UHttpPluginBPLibrary::AsyncRequestHTTP(
	UObject* WorldContextObject,
	FString URL) {
	// Create Action Instance for Blueprint System
	UHttpPluginBPLibrary* Action = NewObject<UHttpPluginBPLibrary>();
	Action->URL = URL;
	Action->RegisterWithGameInstance(WorldContextObject);

	return Action;
}
