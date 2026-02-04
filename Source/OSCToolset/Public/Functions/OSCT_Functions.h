// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OSCT_Functions.generated.h"


/**
 * 
 */
UCLASS()
class OSCTOOLSET_API UOSCT_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Safely converts any Enum to a string without crashing the Editor */
	template<typename T>
	static FString GetEnumString(T EnumValue);

	static FString CleanupAddress(const FString& InAddress);
	
	UFUNCTION(BlueprintCallable, Category="OSCToolset")
	static bool FormatAddress(const FOSCT_Module& Module, FString& OutAddress);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Networking")
	static void SendModuleStateUpdate(UOSCClient* Client, const FOSCT_Module& Data, UObject* Context, bool bIsConnecting);
	

};
