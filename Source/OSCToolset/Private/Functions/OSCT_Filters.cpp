// Fill out your copyright notice in the Description page of Project Settings.


#include "Functions/OSCT_Filters.h"

#include "OSCToolsetLog.h"

void UOSCT_Filters::FilterByAddress(const FOSCT_Module& InModule, const FString& MatchAddress, EOSCT_FilterResult& Branches)
{
	// const FString& ReceiverFullAddr = InModule.GetFormattedAddress();
    
	if (InModule.FormattedAddress.IsEmpty())
	{
		Branches = EOSCT_FilterResult::NoMatch;
		return;
	}
	
	// Comparing references is much faster than copying then comparing
	if (InModule.FormattedAddress == MatchAddress)
	{
		Branches = EOSCT_FilterResult::Matches;
		return;
	}

	// Check if the "Clean" version matches (User passed a raw address like "my/path")
	// We compare the end of the Receiver's full address to the MatchAddress
	if (InModule.FormattedAddress.EndsWith(InModule.Address))
	{
		Branches = EOSCT_FilterResult::Matches;
		return;
	}

	Branches = EOSCT_FilterResult::NoMatch;
}

bool UOSCT_Filters::FilterByModule(const FOSCT_Module& InModule, const FOSCT_Module& MatchModule, EOSCT_FilterResult& Branches)
{
	// These calls will now trigger the calculation automatically if needed
	// const FString& AddrA = InModule.GetFormattedAddress();
	// const FString& AddrB = MatchModule.GetFormattedAddress();

	const bool bMatches = (!InModule.FormattedAddress.IsEmpty() && InModule.FormattedAddress == MatchModule.FormattedAddress);
	UE_LOG(OSCToolset, Log, TEXT("In: %s Match: %s"), *InModule.FormattedAddress, *MatchModule.FormattedAddress)
    
	Branches = bMatches ? EOSCT_FilterResult::Matches : EOSCT_FilterResult::NoMatch;
	return bMatches;
}