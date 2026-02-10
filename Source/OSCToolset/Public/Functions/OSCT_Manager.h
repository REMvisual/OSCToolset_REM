// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OSCT_ETypes.h"
#include "OSCT_Modules.h"

#include "OSCT_Manager.generated.h"

/**
 * 
 */
UCLASS()
class OSCTOOLSET_API UOSCT_Manager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	static UOSCT_Master* GetMaster(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void AddReceiver(UObject* WorldContextObject, FOSCT_Receiver Receiver, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void AddManyReceivers(UObject* WorldContextObject, TArray<FOSCT_Receiver> Receivers, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void RemoveReceiver(UObject* WorldContextObject, FOSCT_Receiver Receiver, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendEvent(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendFloat(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const float Value, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendString(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FString& Value, UObject* Owner);

};
