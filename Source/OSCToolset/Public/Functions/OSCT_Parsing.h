// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OSCManager.h"
#include "OSCT_ETypes.h"

#include "OSCT_Parsing.generated.h"

/**
 * 
 */
UCLASS()
class OSCTOOLSET_API UOSCT_Parsing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool TryGetEvent(const FOSCMessage& InMessage, bool& OutValue);
	static bool TryGetEventPack(const FOSCMessage& InMessage, TMap < FString, bool>& OutMap);
	
	static bool TryGetFloat(const FOSCMessage& InMessage, float& OutValue);
	static bool TryGetFloatPack(const FOSCMessage& InMessage, TMap < FString, float >& OutMap);
	
	static bool TryGetVector2(const FOSCMessage& InMessage, FVector2D& OutValue);
	
	static bool TryGetVector3(const FOSCMessage& InMessage, FVector& OutValue);
	static bool TryGetFloatPack(const FOSCMessage& InMessage, TMap < FString, FVector >& OutMap);
	
	static bool TryGetRotation(const FOSCMessage& InMessage, FRotator& OutValue);

	static bool TryGetColor(const FOSCMessage& InMessage, FLinearColor& OutValue);
	
	static bool TryGetTransform(const FOSCMessage& InMessage, FTransform& OutValue);
	
	static bool TryGetString(const FOSCMessage& InMessage, FString& OutValue);
	
	static bool TryGetNotes(const FOSCMessage& InMessage, FOSCT_Note& OutValue);
	
};
