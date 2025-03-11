// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Transform.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransform, FString, Address, FTransform, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransformTick, FString, Address, FTransform, Value);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Transform : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Transform();

	FTransform Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetTransform Get_Transform;

	UPROPERTY(BlueprintAssignable)
	FOnGetTransformTick Get_Transform_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FTransform TargetValue;
};
