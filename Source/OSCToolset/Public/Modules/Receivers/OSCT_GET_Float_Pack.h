// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Float_Pack.generated.h"

USTRUCT(BlueprintType)
struct FFloatPack
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
    TMap < FString, float > Map;

};

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFloatPack, FString, Address, const FFloatPack, Pack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFloatPackTick, FString, Address, const FFloatPack, Pack);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Float_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Float_Pack();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FFloatPack Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetFloatPack Get_Float_Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetFloatPackTick Get_Float_Pack_Tick;

    virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FFloatPack TargetPack;
	TMap<FString, float> TargetValues;
};
