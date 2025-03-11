// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Color_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Color_Pack::UOSCT_GET_Color_Pack()
{
	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::COLOR;
	SetDebugColor();

	isPack = true;
}

void UOSCT_GET_Color_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	const int32 compLength = 4;
	int32 length = GetMessagePackLength(InMessage, compLength);

	TMap < FString, FLinearColor > ColorMap;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i * (compLength + 1));
		//UE_LOG(OSCToolset, Error, TEXT("CompLength: %d"), compLength);
		FString key = "";
		UOSCManager::GetString(InMessage, index, key);

		FLinearColor val;
		float tempR = 0.0f, tempG = 0.0f, tempB = 0.0f, tempA = 0.0f;

		UOSCManager::GetFloat(InMessage, index + 1, tempR);
		UOSCManager::GetFloat(InMessage, index + 2, tempG);
		UOSCManager::GetFloat(InMessage, index + 3, tempB);
		UOSCManager::GetFloat(InMessage, index + 4, tempA);

		//UE_LOG(OSCToolset, Error, TEXT("/*Index*/: %d -> Key:%s"), index, *key);

		val = FLinearColor(tempR, tempG, tempB, tempA);

		ColorMap.Add(key, val);
	}
	Pack.Map = ColorMap;
	Get_Color_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);

}

void UOSCT_GET_Color_Pack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUpdated)
	{
		for (const TPair<FString, FLinearColor>& Elem : Pack.Map)
		{
			// Initialize the TargetValue for the current key if it hasn't been set yet
			if (!TargetValues.Contains(Elem.Key))
			{
				TargetValues.Add(Elem.Key, FLinearColor::White); // Use white to initialize
			}
			TargetValues[Elem.Key] = FMath::CInterpTo(TargetValues[Elem.Key], Elem.Value, DeltaTime, InterpolationSpeed);
		}
		TargetPack.Map = TargetValues; // Update the target pack with the new values
		Get_Color_Pack_Tick.Broadcast(Address, TargetPack); // Broadcast the updated target pack
	}
}
