// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module.h"
#include "OSCT_Module_Receiver.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOSCMessageFiltered, const FOSCMessage&, Message, const FString&, IPAddress, int32, Port);

UCLASS(Abstract)
class OSCTOOLSET_API UOSCT_Module_Receiver : public UOSCT_Module
{
	GENERATED_BODY()

public:
	UOSCT_Module_Receiver();

	UPROPERTY()
	bool firstMessage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", meta = (EditCondition = "ModuleType == EOSCT_Module_Type::FLOAT || ModuleType == EOSCT_Module_Type::VEC2 || ModuleType == EOSCT_Module_Type::VEC3 || ModuleType == EOSCT_Module_Type::COLOR || ModuleType == EOSCT_Module_Type::TRANSFORM || ModuleType == EOSCT_Module_Type::ROTATION", EditConditionHides))
	bool EnableTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", meta = (EditCondition = "ModuleType == EOSCT_Module_Type::FLOAT || ModuleType == EOSCT_Module_Type::VEC2 || ModuleType == EOSCT_Module_Type::VEC3 || ModuleType == EOSCT_Module_Type::COLOR || ModuleType == EOSCT_Module_Type::TRANSFORM || ModuleType == EOSCT_Module_Type::ROTATION", EditConditionHides))
	float InterpolationSpeed;

	UPROPERTY()
	FOSCMessageFiltered OnMessageFiltered;

	UFUNCTION()
	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);

protected:
	//Check for incoming messages timer
	bool bUpdated;                     // Tracks whether messages are being received
	FTimerHandle MessageTimeoutHandle; // Timer handle to manage the cooldown timer
	float MessageTimeoutDuration;      // The duration to wait before marking as "stopped"

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void init_OSCT_Module() override;

	virtual void shutdown_OSCT_Module() override;

	// Function to reset the timer
	void ResetMessageTimer();

	// Function to be called when the timer runs out
	void OnMessageTimeout();

	int32 GetMessagePackLength(const FOSCMessage& InMessage, const int32 ComponentLength);

	UFUNCTION()
	void filter_OSC_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);
};
