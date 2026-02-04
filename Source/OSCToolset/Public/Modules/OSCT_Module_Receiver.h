// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_ModuleComponent.h"
#include "Interfaces/OSCT_Router.h"

#include "OSCT_Module_Receiver.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOSCMessageFiltered, const FOSCMessage&, Message, const FString&, IPAddress, int32, Port);

UCLASS(Abstract)
class OSCTOOLSET_API UOSCT_Module_Receiver : public UOSCT_ModuleComponent, public IOSCT_Router
{
	GENERATED_BODY()

public:
	UOSCT_Module_Receiver();

	UPROPERTY()
	bool firstMessage = false;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", meta = (EditCondition = "ModuleType == EOSCT_ModuleType::FLOAT || ModuleType == EOSCT_ModuleType::VEC2 || ModuleType == EOSCT_ModuleType::VEC3 || ModuleType == EOSCT_ModuleType::COLOR || ModuleType == EOSCT_ModuleType::TRANSFORM || ModuleType == EOSCT_ModuleType::ROTATION", EditConditionHides))
	// bool EnableTick;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", meta = (EditCondition = "ModuleType == EOSCT_ModuleType::FLOAT || ModuleType == EOSCT_ModuleType::VEC2 || ModuleType == EOSCT_ModuleType::VEC3 || ModuleType == EOSCT_ModuleType::COLOR || ModuleType == EOSCT_ModuleType::TRANSFORM || ModuleType == EOSCT_ModuleType::ROTATION", EditConditionHides))
	// float InterpolationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", 
		meta = (EditCondition = "ModuleType != EOSCT_ModuleType::EVENT && ModuleType != EOSCT_ModuleType::NOTE", EditConditionHides))
	bool EnableTick;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", 
		meta = (EditCondition = "ModuleType != EOSCT_ModuleType::EVENT && ModuleType != EOSCT_ModuleType::NOTE", EditConditionHides))
	float InterpolationSpeed;
	
	UPROPERTY()
	FOSCMessageFiltered OnMessageFiltered;

	UFUNCTION()
	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);
	
	//New interface message handling
	virtual void OnOSCMessageReceived_Implementation(const FString& Address, const FOSCMessage& Message) override;
	
protected:
	int32 componentLength;			   // Sets the length of the module component. 

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

	int32 GetMessagePackLength(const FOSCMessage& InMessage);

	UFUNCTION()
	void filter_OSC_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);
};
