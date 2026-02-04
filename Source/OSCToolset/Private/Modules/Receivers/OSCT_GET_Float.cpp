// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Float.h"

#include "OSCManager.h"

UOSCT_GET_Float::UOSCT_GET_Float()
{
	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::FLOAT;
	SetDebugColor();
}

void UOSCT_GET_Float::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	UOSCManager::GetFloat(InMessage, 0, Value);
	Get_Float.Broadcast(Address, Value);

	// DEBUG //
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);  // Limit to 2 decimal places
	OSCTDebugOSCMessage(ValueString);
}

// Called every frame
void UOSCT_GET_Float::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bUpdated)
	{
		TargetValue = FMath::FInterpTo(TargetValue, Value, DeltaTime, InterpolationSpeed);
		Get_Float_Tick.Broadcast(Address, TargetValue);
	}
}
