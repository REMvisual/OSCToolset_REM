// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Vector2.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVector2, FString, Address, FVector2D, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVector2Tick, FString, Address, FVector2D, Value);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Vector2 : public UOSCT_Module_Receiver
{
	GENERATED_BODY()
	
public:
	UOSCT_GET_Vector2();

	FVector2D Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetVector2 Get_Vector2;	

	UPROPERTY(BlueprintAssignable)
	FOnGetVector2Tick Get_Vector2_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVector2D TargetValue;
};