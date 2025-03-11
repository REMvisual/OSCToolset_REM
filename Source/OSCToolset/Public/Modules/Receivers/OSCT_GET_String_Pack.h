// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_String_Pack.generated.h"


//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetStringPack, const TArray<FString>, Pack);


UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_String_Pack : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_String_Pack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	TArray<FString> Pack;

	UPROPERTY(BlueprintAssignable)
	FOnGetStringPack Get_String_Pack;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;
};
