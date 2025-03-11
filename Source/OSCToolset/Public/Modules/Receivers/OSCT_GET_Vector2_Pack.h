// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Vector2_Pack.generated.h"

USTRUCT(BlueprintType)
struct FVec2Pack
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
    TMap < FString, FVector2D > Map;

};
//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVec2Pack, FString, Address, const FVec2Pack, Pack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVec2PackTick, FString, Address, const FVec2Pack, Pack);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Vector2_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Vector2_Pack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FVec2Pack Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetVec2Pack Get_Vector2_Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetVec2PackTick Get_Vector2_Pack_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVec2Pack TargetPack;
	TMap<FString, FVector2D> TargetValues;

};
