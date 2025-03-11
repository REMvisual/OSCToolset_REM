// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Rotation.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetRotation, FString, Address, FRotator, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetRotationTick, FString, Address, FRotator, Value);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Rotation : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Rotation();

	FRotator Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetRotation Get_Rotation;

	UPROPERTY(BlueprintAssignable)
	FOnGetRotationTick Get_Rotation_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FRotator TargetValue;

};
