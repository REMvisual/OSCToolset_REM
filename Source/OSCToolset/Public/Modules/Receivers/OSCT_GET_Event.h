// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Event.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetEvent, FString, Address);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Event : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Event();

	UPROPERTY(BlueprintAssignable)
	FOnGetEvent Get_Event;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

};
