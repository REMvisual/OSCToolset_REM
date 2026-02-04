// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Vector2_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Vector2_Pack::UOSCT_GET_Vector2_Pack()
{
	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::VEC2;
	componentLength = 2;
	SetDebugColor();

	isPack = true;
}

void UOSCT_GET_Vector2_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	int32 length = GetMessagePackLength(InMessage);

	TMap < FString, FVector2D > Vec2Map;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i * (componentLength + 1)) + 1;
		//UE_LOG(OSCToolset, Error, TEXT("CompLength: %d"), compLength);
		FString key = "";
		UOSCManager::GetString(InMessage, index, key);

		FVector2D val;
		float tempX = 0.0f, tempY = 0.0f;

		UOSCManager::GetFloat(InMessage, index+1, tempX);
		UOSCManager::GetFloat(InMessage, index+2, tempY);

		//UE_LOG(OSCToolset, Error, TEXT("Index: %d -> Key:%s"), index, *key);

		val.Set(tempX, tempY);

		Vec2Map.Add(key, val);
	}
	Pack.Map = Vec2Map;
	Get_Vector2_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);
}

void UOSCT_GET_Vector2_Pack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUpdated)
	{
		for (const TPair<FString, FVector2D>& Elem : Pack.Map)
		{
			// Initialize the TargetValue for the current key if it hasn't been set yet
			if (!TargetValues.Contains(Elem.Key))
			{
				TargetValues.Add(Elem.Key, FVector2D::ZeroVector); // Use white to initialize
			}
			TargetValues[Elem.Key] = FMath::Vector2DInterpTo(TargetValues[Elem.Key], Elem.Value, DeltaTime, InterpolationSpeed);
		}
		TargetPack.Map = TargetValues; // Update the target pack with the new values
		Get_Vector2_Pack_Tick.Broadcast(Address, TargetPack); // Broadcast the updated target pack
	}
}
