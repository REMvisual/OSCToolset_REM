// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Event_Pack.generated.h"

USTRUCT(BlueprintType)
struct FEventPack
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
    TMap < FString, bool > Map;

};

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetEventPack, FString, Address, const FEventPack, Pack);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Event_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
    UOSCT_GET_Event_Pack();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
    FEventPack Pack;

    UPROPERTY(BlueprintAssignable)
    FOnGetEventPack Get_Event_Pack;

    virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

};
