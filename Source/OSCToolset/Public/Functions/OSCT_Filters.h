// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OSCT_ETypes.h"

#include "OSCT_Filters.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EOSCT_FilterResult : uint8
{
	Matches,
	NoMatch
};

UCLASS()
class OSCTOOLSET_API UOSCT_Filters : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Filters", meta = (ExpandEnumAsExecs = "Branches"))
	static void FilterByAddress(const FOSCT_Module& InModule, const FString& MatchAddress, EOSCT_FilterResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Filters", meta = (ExpandEnumAsExecs = "Branches"))
	static bool FilterByModule(const FOSCT_Module& InModule, const FOSCT_Module& MatchModule, EOSCT_FilterResult& Branches);
};
