// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "OSCServer.h"
#include "OSCClient.h"

#include "Interfaces/OSCT_Listener.h"

#include "OSCT_Settings.h"
#include "OSCT_ETypes.h"

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

	UFUNCTION(BlueprintCallable, Category = "OSCToolset")
	void RegisterListener(FString Address, UObject* Listener);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset")
	void UnregisterListener(FString Address, UObject* Listener);
	
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
protected:
	// Event handler for the level change
	void OnLevelChanged(const FString& LevelName);

private:

	UPROPERTY()
	FString IPV4;

	UFUNCTION()
	void init_OSCT_Master();

	UFUNCTION()
	void shutdown_OSCT_Master();

	void reinit_OSCT_Master();
	
	// Maps an OSC Address to a list of Objects that implement IOSCT_Listener
    TMap<FString, TArray<UObject*>> AddressMap;

    // The function bound to OSCT_Server->OnOscMessageReceived
    UFUNCTION()
    void RouteMessage(const FOSCMessage& Message, const FString& IPAddress, int32 Port);
	
	
	FString OSCT_Base_addr = "/OSCT/";

	FString OSCT_Init_addr = OSCT_Base_addr + "init";

	FString OSCT_Shutdown_addr = OSCT_Base_addr + "shutdown";

	FString OSCT_OnLevelChanged_addr = OSCT_Base_addr + "level_changed";

	void InitializeOSC();

	void SendOSCTBaseMessage(FString Message);

	FString GetLocalIPAddress();

	FString SetLocalIPAddress(FString InAddress, bool Log = false);

	UFUNCTION()
	void SetCommands(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);

	void LogSettings();

	TSharedPtr<SOSCT_Menu> OSCTMenu;
};