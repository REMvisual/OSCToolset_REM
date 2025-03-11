// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Float.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFloat, FString, Address, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFloatTick, FString, Address, float, Value);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Float : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Float();

	float Value = 0.0f;

	UPROPERTY(BlueprintAssignable)
	FOnGetFloat Get_Float;

	UPROPERTY(BlueprintAssignable)
	FOnGetFloatTick Get_Float_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float TargetValue;
};
	