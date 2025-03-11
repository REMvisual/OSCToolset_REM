// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Transform_Pack.generated.h"

USTRUCT(BlueprintType)
struct FTransformPack
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
    TMap < FString, FTransform > Map;

};
//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransformPack, FString, Address, const FTransformPack, Pack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransformPackTick, FString, Address, const FTransformPack, Pack);


UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Transform_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Transform_Pack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FTransformPack Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetTransformPack Get_Transform_Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetTransformPackTick Get_Transform_Pack_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FTransformPack TargetPack;
	TMap<FString, FTransform> TargetValues;
};
