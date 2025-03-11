// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "OSCServer.h"
#include "OSCClient.h"

#include "OSCT_Settings.h"
#include "UI/SOSCT_Menu.h"

#include "OSCT_Master.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(OSCToolset, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOSCTInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOSCTShutdown);

UCLASS()
class OSCTOOLSET_API UOSCT_Master : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category="OSCToolset")
	UOSCServer* OSCT_Server;

	UPROPERTY(BlueprintReadOnly, Category = "OSCToolset")
	UOSCClient* OSCT_Client;

	// Delegate for the Init OSC.
	UPROPERTY()
	FOnOSCTInit OnInitOSCT;

	UPROPERTY()
	FOnOSCTShutdown OnShutdownOSCT;

	UPROPERTY()
	const UOSCT_Settings* Settings;

	// Initialize the subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Deinitialize or clean up the subsystem
	virtual void Deinitialize() override;
	
	void ToggleOSCTMenu();

private:

	UPROPERTY()
	FString IPV4;

	UFUNCTION()
	void init_OSCT_Master();

	UFUNCTION()
	void shutdown_OSCT_Master();

	void reinit_OSCT_Master();

	FString OSCT_Base_addr = "/osctoolset_";

	FString OSCT_Init_addr = OSCT_Base_addr + "init";

	FString OSCT_Shutdown_addr = OSCT_Base_addr + "shutdown";

	void InitializeOSC();

	void SendOSCTBaseMessage(FString Message);

	FString GetLocalIPAddress();

	FString SetLocalIPAddress(FString InAddress, bool Log = false);

	UFUNCTION()
	void SetCommands(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);

	void LogSettings();

	TSharedPtr<SOSCT_Menu> OSCTMenu;
};