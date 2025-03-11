// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Transform_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Transform_Pack::UOSCT_GET_Transform_Pack()
{
	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::TRANSFORM;
	componentLength = 9;
	SetDebugColor();

	isPack = true;
}

void UOSCT_GET_Transform_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	int32 length = GetMessagePackLength(InMessage);

	TMap < FString, FTransform > TransformMap;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i * (componentLength + 1)) + 1;
		//UE_LOG(OSCToolset, Error, TEXT("CompLength: %d"), compLength);
		FString key = "";
		UOSCManager::GetString(InMessage, index, key);

		FTransform val;

		float tempTX = 0.0f, tempTY = 0.0f, tempTZ = 0.0f;
		float tempRX = 0.0f, tempRY = 0.0f, tempRZ = 0.0f;
		float tempSX = 0.0f, tempSY = 0.0f, tempSZ = 0.0f;

		FVector Location;
		UOSCManager::GetFloat(InMessage, index + 1, tempTX);
		UOSCManager::GetFloat(InMessage, index + 2, tempTY);
		UOSCManager::GetFloat(InMessage, index + 3, tempTZ);
		Location.Set(tempTX, tempTY, tempTZ);
		val.SetLocation(Location);

		// Set the rotation (quaternion from a rotator)
		FQuat Rotation; // = FQuat4f(FRotator3f(45.0f, 90.0f, 0.0f));  // Euler angles to quaternion
		FRotator EurlerRotation;
		UOSCManager::GetFloat(InMessage, index + 4, tempRX);
		UOSCManager::GetFloat(InMessage, index + 5, tempRY);
		UOSCManager::GetFloat(InMessage, index + 6, tempRZ);

		EurlerRotation = FRotator(tempRX, tempRY, tempRZ);

		Rotation = FQuat(EurlerRotation);
		val.SetRotation(Rotation);


		// Set the scale (uniform scale)
		FVector Scale;
		UOSCManager::GetFloat(InMessage, index + 7, tempSX);
		UOSCManager::GetFloat(InMessage, index + 8, tempSY);
		UOSCManager::GetFloat(InMessage, index + 9, tempSZ);

		Scale.Set(tempSX, tempSY, tempSZ);

		val.SetScale3D(Scale);

		//UE_LOG(OSCToolset, Error, TEXT("/*Index*/: %d -> Key:%s"), index, *key);
		TransformMap.Add(key, val);
	}
	Pack.Map = TransformMap;
	Get_Transform_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);

}

void UOSCT_GET_Transform_Pack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUpdated)
	{
		for (const TPair<FString, FTransform>& Elem : Pack.Map)
		{
			// Initialize the TargetValue for the current key if it hasn't been set yet
			if (!TargetValues.Contains(Elem.Key))
			{
				TargetValues.Add(Elem.Key, FTransform::Identity); // Use Identity to initialize
			}

			// Get the current target transform for this key
			FTransform& CurrentTargetTransform = TargetValues[Elem.Key];

			// Interpolate each component of the transform
			FVector NewLocation = FMath::VInterpTo(CurrentTargetTransform.GetLocation(), Elem.Value.GetLocation(), DeltaTime, InterpolationSpeed);
			FQuat NewRotation = FQuat::Slerp(CurrentTargetTransform.GetRotation(), Elem.Value.GetRotation(), DeltaTime * InterpolationSpeed);
			FVector NewScale = FMath::VInterpTo(CurrentTargetTransform.GetScale3D(), Elem.Value.GetScale3D(), DeltaTime, InterpolationSpeed);

			// Update the target transform with the new interpolated values
			CurrentTargetTransform.SetLocation(NewLocation);
			CurrentTargetTransform.SetRotation(NewRotation);
			CurrentTargetTransform.SetScale3D(NewScale);
		}
		TargetPack.Map = TargetValues; // Update the target pack with the new values
		Get_Transform_Pack_Tick.Broadcast(Address, TargetPack); // Broadcast the updated target pack
	}
}
