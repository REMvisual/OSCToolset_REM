// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved.


#include "Modules/Receivers/OSCT_GET_Rotation_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Rotation_Pack::UOSCT_GET_Rotation_Pack()
{
	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::ROTATION;
	componentLength = 3;
	SetDebugColor();

	isPack = true;
}

void UOSCT_GET_Rotation_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	int32 length = GetMessagePackLength(InMessage);

	TMap < FString, FRotator > RotationMap;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i * (componentLength + 1)) + 1;

		FString key = "";
		UOSCManager::GetString(InMessage, index, key);

		FRotator val;
		float tempX = 0.0f, tempY = 0.0f, tempZ = 0.0f;

		UOSCManager::GetFloat(InMessage, index + 1, tempX);
		UOSCManager::GetFloat(InMessage, index + 2, tempY);
		UOSCManager::GetFloat(InMessage, index + 3, tempZ);

		//UE_LOG(OSCToolset, Error, TEXT("Index: %d -> Key:%s"), index, *key);

		//val.Set(tempX, tempY, tempZ);
		val.Roll = tempX;
		val.Pitch = tempY;
		val.Yaw = tempZ;

		RotationMap.Add(key, val);
	}
	Pack.Map = RotationMap;
	Get_Rotation_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);
}

void UOSCT_GET_Rotation_Pack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bUpdated)
	{
		for (const TPair<FString, FRotator>& Elem : Pack.Map)
		{
			// Initialize the TargetValue for the current key if it hasn't been set yet
			if (!TargetValues.Contains(Elem.Key))
			{
				TargetValues.Add(Elem.Key, FRotator::ZeroRotator); // Use white to initialize
			}
			TargetValues[Elem.Key] = FMath::RInterpTo(TargetValues[Elem.Key], Elem.Value, DeltaTime, InterpolationSpeed);
		}
		TargetPack.Map = TargetValues; // Update the target pack with the new values
		Get_Rotation_Pack_Tick.Broadcast(Address, TargetPack); // Broadcast the updated target pack
	}

}
