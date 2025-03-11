// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Color_Pack.generated.h"

USTRUCT(BlueprintType)
struct FColorPack
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
    TMap < FString, FLinearColor > Map;

};
//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetColorPack, FString, Address, const FColorPack, Pack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetColorPackTick, FString, Address, const FColorPack, Pack);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Color_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Color_Pack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FColorPack Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetColorPack Get_Color_Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetColorPackTick Get_Color_Pack_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FColorPack TargetPack;
	TMap<FString, FLinearColor> TargetValues;

};
