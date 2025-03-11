// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved.


#include "Modules/Receivers/OSCT_GET_Rotation.h"
#include "OSCManager.h"

UOSCT_GET_Rotation::UOSCT_GET_Rotation()
{
	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::ROTATION;
	SetDebugColor();
}

void UOSCT_GET_Rotation::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	float tempX = 0.0f, tempY = 0.0f, tempZ = 0.0f;
	UOSCManager::GetFloat(InMessage, 0, tempX);
	UOSCManager::GetFloat(InMessage, 1, tempY);
	UOSCManager::GetFloat(InMessage, 2, tempZ);

	Value.Roll = tempX;  // X-axis rotation
	Value.Pitch = tempY;    // Y-axis rotation
	Value.Yaw = tempZ;    // Z-axis rotation

	Get_Rotation.Broadcast(Address, Value);

	// DEBUG //
	FString ValueString = FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"), Value.Roll, Value.Pitch, Value.Yaw);  // Limit to 2 decimal places
	OSCTDebugOSCMessage(ValueString);

}

void UOSCT_GET_Rotation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUpdated)
	{
		TargetValue = FMath::RInterpTo(TargetValue, Value, DeltaTime, InterpolationSpeed);
		Get_Rotation_Tick.Broadcast(Address, TargetValue);
	}
}
