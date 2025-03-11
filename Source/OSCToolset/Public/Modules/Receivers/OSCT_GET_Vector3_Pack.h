// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Vector3_Pack.generated.h"

USTRUCT(BlueprintType)
struct FVec3Pack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	TMap < FString, FVector > Map;

};
//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVec3Pack, FString, Address, const FVec3Pack, Pack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVec3PackTick, FString, Address, const FVec3Pack, Pack);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Vector3_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_Vector3_Pack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FVec3Pack Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetVec3Pack Get_Vector3_Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetVec3PackTick Get_Vector3_Pack_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVec3Pack TargetPack;
	TMap<FString, FVector> TargetValues;
};
