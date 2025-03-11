// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Float_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Float_Pack::UOSCT_GET_Float_Pack()
{
	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::FLOAT;
	SetDebugColor();

	isPack = true;

}

void UOSCT_GET_Float_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	const int32 compLength = 1;
	int32 length = GetMessagePackLength(InMessage, compLength);

	TMap < FString, float > FloatMap;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i) * 2;
		
		FString key = "";
		float val = 0.0f;

		UOSCManager::GetString(InMessage, index, key);	// Pair (even) index, sequence of 0-2-4-6-...
		UOSCManager::GetFloat(InMessage, index+1, val);	// Odd index is i + 1 1-3-5--9,...7

		FloatMap.Add(key, val);
	}

	Pack.Map = FloatMap;
	Get_Float_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);

}

void UOSCT_GET_Float_Pack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUpdated) 
	{
		for (const TPair<FString, float>& Elem : Pack.Map)
		{
			// Initialize the TargetValue for the current key if it hasn't been set yet
			if (!TargetValues.Contains(Elem.Key))
			{
				TargetValues.Add(Elem.Key, 0.0f); // or any default starting value
			}
			// Interpolate the value for the current key
			TargetValues[Elem.Key] = FMath::FInterpTo(TargetValues[Elem.Key], Elem.Value, DeltaTime, InterpolationSpeed);
		}

		TargetPack.Map = TargetValues;
		Get_Float_Pack_Tick.Broadcast(Address, TargetPack);
	}
}
	