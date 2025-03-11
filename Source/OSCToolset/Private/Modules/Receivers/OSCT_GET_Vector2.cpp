// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Vector2.h"

#include "OSCManager.h"

UOSCT_GET_Vector2::UOSCT_GET_Vector2()
{
	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::VEC2;
	SetDebugColor();
}

void UOSCT_GET_Vector2::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	float tempX = 0.0f, tempY = 0.0f;

	UOSCManager::GetFloat(InMessage, 0, tempX);
	UOSCManager::GetFloat(InMessage, 1, tempY);

	Value.Set(tempX, tempY);

	Get_Vector2.Broadcast(Address, Value);

	// DEBUG //
	FString ValueString = FString::Printf(TEXT("X: %.2f Y:%.2f"), Value.X, Value.Y);  // Limit to 2 decimal places
	OSCTDebugOSCMessage(ValueString);
}

void UOSCT_GET_Vector2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bUpdated)
	{
		TargetValue = FMath::Vector2DInterpTo(TargetValue, Value, DeltaTime, InterpolationSpeed);
		Get_Vector2_Tick.Broadcast(Address, TargetValue);
	}
}
