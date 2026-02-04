// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Color.h"

#include "OSCManager.h"

UOSCT_GET_Color::UOSCT_GET_Color()
{
	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::COLOR;
	SetDebugColor();
}

void UOSCT_GET_Color::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	UOSCManager::GetFloat(InMessage, 0, Value.R);
	UOSCManager::GetFloat(InMessage, 1, Value.G);
	UOSCManager::GetFloat(InMessage, 2, Value.B);
	UOSCManager::GetFloat(InMessage, 3, Value.A);

	Get_Color.Broadcast(Address, Value);

	// DEBUG //
	FString ValueString = FString::Printf(TEXT("R: %.2f G: %.2f B: %.2f A: %.2f"), Value.R, Value.G, Value.B, Value.A);  // Limit to 2 decimal places
	OSCTDebugOSCMessage(ValueString);
}

void UOSCT_GET_Color::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bUpdated)
	{
		TargetValue = FMath::CInterpTo(TargetValue, Value, DeltaTime, InterpolationSpeed);
		Get_Color_Tick.Broadcast(Address, TargetValue);
	}
}
