// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_String.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetString, FString, Address, FString, Value);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_String : public UOSCT_Module_Receiver
{
	GENERATED_BODY()
	
public:
	UOSCT_GET_String();

	FString Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetString Get_String;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

};
