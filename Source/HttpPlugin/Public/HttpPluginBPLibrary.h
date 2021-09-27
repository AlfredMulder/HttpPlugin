// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpPluginBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

// Event that will be the 'Completed' exec wire in the blueprint node along with all parameters as output pins.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHttpRequestCompleted,
                                               const FString&, CharacterInfo,
                                               bool, bSuccess,
                                               const FString&, Image_URL);

UCLASS()
class UHttpPluginBPLibrary final : public UBlueprintAsyncActionBase {
	GENERATED_UCLASS_BODY()
protected:
	void HandleRequestCompleted(FString ResponseString, bool bSuccess, FString ImageURL) const;

public:
	/** Execute the actual load */
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable,
		meta = (BlueprintInternalUseOnly = "true", Category = "HTTP", WorldContext =
			"WorldContextObject"))
	static UHttpPluginBPLibrary* AsyncRequestHTTP(
		UObject* WorldContextObject,
		FString URL);

	static void ProcessingOutputString(FString& ResponseString, TSharedPtr<FJsonObject> JsonObject);

	UPROPERTY(BlueprintAssignable)
	FOnHttpRequestCompleted Completed;

	/* URL to send GET request to */
	FString URL;
};
