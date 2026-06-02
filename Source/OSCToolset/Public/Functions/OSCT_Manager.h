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

	// Hidden from the Blueprint palette (BlueprintInternalUseOnly) — the "Add OSC Receivers" node is the public face.
	// Still callable: the K2Node's expansion references this by name.
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner", BlueprintInternalUseOnly = "true"))
	static void AddReceiversFromDataTable(UObject* WorldContextObject, UDataTable* InTable, UObject* Owner, const FString& AddressFilter = TEXT(""));
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void RemoveReceiver(UObject* WorldContextObject, FOSCT_Receiver Receiver, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (WorldContext = "WorldContextObject"))
	static void RemoveAllReceivers(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendEvent(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendBool(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const bool Value, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendFloat(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const float Value, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendInteger(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const int32 Value, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendVector2(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FVector2D Value, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendVector3(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FVector Value, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendRotation(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FRotator Value, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendColor(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FLinearColor Value, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendTransform(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FTransform Value, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Senders", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static void SendString(UObject * WorldContextObject, UPARAM(ref) FOSCT_Sender& Sender, const FString& Value, UObject* Owner);

public:
	//// LIVE VALUE GETTERS
	// Poll the live value for a receiver Address on this Owner. The receiver must already be registered
	// (e.g. via "Add Receivers From Data Table" on BeginPlay). Returns false until a value has been received.
	// These power the "OSC Switch Values" node, but are fully usable on their own.
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCFloat(UObject* WorldContextObject, const FString& Address, float& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCInteger(UObject* WorldContextObject, const FString& Address, int32& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCVector2(UObject* WorldContextObject, const FString& Address, FVector2D& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCVector3(UObject* WorldContextObject, const FString& Address, FVector& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCRotation(UObject* WorldContextObject, const FString& Address, FRotator& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCColor(UObject* WorldContextObject, const FString& Address, FLinearColor& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCTransform(UObject* WorldContextObject, const FString& Address, FTransform& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCBool(UObject* WorldContextObject, const FString& Address, bool& OutValue, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Get", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Owner", HidePin = "Owner"))
	static bool GetOSCString(UObject* WorldContextObject, const FString& Address, FString& OutValue, UObject* Owner);

};
