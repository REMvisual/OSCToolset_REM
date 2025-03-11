// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_Rotation_Pack.generated.h"

USTRUCT(BlueprintType)
struct FRotationPack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	TMap < FString, FRotator > Map;

};
//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetRotationPack, FString, Address, const FRotationPack, Pack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetRotationPackTick, FString, Address, const FRotationPack, Pack);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_Rotation_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()
public:
	UOSCT_GET_Rotation_Pack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FRotationPack Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetRotationPack Get_Rotation_Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetRotationPackTick Get_Rotation_Pack_Tick;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FRotationPack TargetPack;
	TMap<FString, FRotator> TargetValues;

};
