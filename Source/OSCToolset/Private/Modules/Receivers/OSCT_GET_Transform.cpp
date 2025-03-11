// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Transform.h"

#include "OSCManager.h"

#include "Math/Transform.h"
#include "Math/UnrealMathUtility.h"

UOSCT_GET_Transform::UOSCT_GET_Transform()
{
	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::TRANSFORM;
	SetDebugColor();
}

void UOSCT_GET_Transform::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	float tempTX = 0.0f, tempTY = 0.0f, tempTZ = 0.0f;
	float tempRX = 0.0f, tempRY = 0.0f, tempRZ = 0.0f;
	float tempSX = 0.0f, tempSY = 0.0f, tempSZ = 0.0f;

	// Set the location (translation)
	FVector Location; // (100.0f, 200.0f, 300.0f);
	UOSCManager::GetFloat(InMessage, 0, tempTX);
	UOSCManager::GetFloat(InMessage, 1, tempTY);
	UOSCManager::GetFloat(InMessage, 2, tempTZ);

	Location.Set(tempTX, tempTY, tempTZ);
	Value.SetLocation(Location);

	// Set the rotation (quaternion from a rotator)
	FQuat Rotation; // = FQuat4f(FRotator3f(45.0f, 90.0f, 0.0f));  // Euler angles to quaternion
	FRotator EurlerRotation;
	UOSCManager::GetFloat(InMessage, 3, tempRX);
	UOSCManager::GetFloat(InMessage, 4, tempRY);
	UOSCManager::GetFloat(InMessage, 5, tempRZ);

	EurlerRotation = FRotator(tempRX, tempRY, tempRZ);

	Rotation = FQuat(EurlerRotation);
	Value.SetRotation(Rotation);

	// Set the scale (uniform scale)
	FVector Scale;
	UOSCManager::GetFloat(InMessage, 6, tempSX);
	UOSCManager::GetFloat(InMessage, 7, tempSY);
	UOSCManager::GetFloat(InMessage, 8, tempSZ);

	Scale.Set(tempSX, tempSY, tempSZ);

	Value.SetScale3D(Scale);	

	Get_Transform.Broadcast(Address, Value);

	// DEBUG //
	FString ValueString = FString::Printf(TEXT("TX: %.2f TY: %.2f TZ: %.2f RX: %.2f RY: %.2f RZ: %.2f SX: %.2f SY: %.2f SZ: %.2f"),tempTX, tempTY, tempTZ, tempRX, tempRY, tempRZ, tempSX, tempSY, tempSZ );  // Limit to 2 decimal places
	OSCTDebugOSCMessage(ValueString);
}

void UOSCT_GET_Transform::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUpdated)
	{
		// Interpolate each component of the transform
		FVector NewLocation = FMath::VInterpTo(TargetValue.GetLocation(), Value.GetLocation(), DeltaTime, InterpolationSpeed);
		FQuat NewRotation = FQuat::Slerp(TargetValue.GetRotation(), Value.GetRotation(), DeltaTime * InterpolationSpeed);
		FVector NewScale = FMath::VInterpTo(TargetValue.GetScale3D(), Value.GetScale3D(), DeltaTime, InterpolationSpeed);

		// Update the target transform with the new interpolated values
		TargetValue.SetLocation(NewLocation);
		TargetValue.SetRotation(NewRotation);
		TargetValue.SetScale3D(NewScale);

		Get_Transform_Tick.Broadcast(Address, TargetValue);
	}
}
