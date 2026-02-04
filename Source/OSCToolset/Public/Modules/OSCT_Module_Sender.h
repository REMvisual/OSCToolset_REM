// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_ModuleComponent.h"
#include "OSCT_Module_Sender.generated.h"

/**
 * 
 */
UCLASS()
class OSCTOOLSET_API UOSCT_Module_Sender : public UOSCT_ModuleComponent
{
	GENERATED_BODY()

public:
	UOSCT_Module_Sender();

	UFUNCTION()
	virtual void SEND_Message();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void init_OSCT_Module() override;

	bool is_name_valid(FString Name);
};
