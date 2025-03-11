// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Sender.h"
#include "OSCT_SEND_Event.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_SEND_Event : public UOSCT_Module_Sender
{
	GENERATED_BODY()

public:
	UOSCT_SEND_Event();

	UFUNCTION(BlueprintCallable, Category = "OSCToolset")
	void OSCT_Send_Event(const FString Name, bool& bSuccess);
};
