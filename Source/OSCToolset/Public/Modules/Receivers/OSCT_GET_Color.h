// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Color.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetColor, FString, Address, FLinearColor, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetColorTick, FString, Address, FLinearColor, Value);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Color : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Color();

	FLinearColor Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetColor Get_Color;

	UPROPERTY(BlueprintAssignable)
	FOnGetColorTick Get_Color_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FLinearColor TargetValue;
};
