// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Vector3.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVector3, FString, Address, FVector, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVector3Tick, FString, Address, FVector, Value);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Vector3 : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Vector3();

	FVector Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetVector3 Get_Vector3;

	UPROPERTY(BlueprintAssignable)
	FOnGetVector3Tick Get_Vector3_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVector TargetValue;
};
 