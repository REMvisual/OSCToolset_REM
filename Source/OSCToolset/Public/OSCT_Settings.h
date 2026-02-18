// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved.
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OSCT_Settings.generated.h"

/**
 * 
 */
UCLASS(config = User, defaultconfig)
class OSCTOOLSET_API UOSCT_Settings : public UObject
{
	GENERATED_BODY()

public:
    UOSCT_Settings();


	UPROPERTY(EditAnywhere, config, category = "OSCToolset|Server (Receive)")
	FString ServerAddress;

	UPROPERTY(EditAnywhere, config, category = "OSCToolset|Server (Receive)")
	int ServerPort;

	UPROPERTY(EditAnywhere, config, category = "OSCToolset|Server (Receive)")
	bool MulticastLoopback;

	UPROPERTY(EditAnywhere, config, category = "OSCToolset|Client (Send)")
	FString ClientAddress;

	UPROPERTY(EditAnywhere, config, category = "OSCToolset|Client (Send)")
	int ClientPort;
	
	UPROPERTY(EditAnywhere, config, category = "OSCToolset")
	bool UseLocalIPV4;
};