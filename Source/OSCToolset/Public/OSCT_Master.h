// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

#include "OSCServer.h"
#include "OSCClient.h"

#include "OSCT_Settings.h"
#include "OSCT_ETypes.h"

#include "UI/SOSCT_Menu.h"

#include "OSCT_Master.generated.h"

// DECLARE_LOG_CATEGORY_EXTERN(OSCToolset, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOSCTInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOSCTShutdown);

UCLASS()
class OSCTOOLSET_API UOSCT_Master : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOSCT_Master, STATGROUP_Tickables); }

public:
	
	UPROPERTY(BlueprintReadOnly, Category="OSCToolset")
	UOSCServer* OSCT_Server;

	UPROPERTY(BlueprintReadOnly, Category = "OSCToolset")
	UOSCClient* OSCT_Client;

	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	void AddModule(FOSCT_Module Module, UObject* Owner);
	
	// UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	// void AddReceiver(FOSCT_Receiver Receiver, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	void AddManyModules(TArray<FOSCT_Module> Modules, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", 
		meta = (
			RequiredAssetDataTag = "RowStructure=/Script/OSCToolset.FOSCT_Receiver",
			DefaultToSelf = "Owner", 
			HidePin = "Owner"))
	void AddReceiversFromDataTable(UDataTable* InTable, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	void RemoveModule(const FOSCT_Module Module, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset")
	void RemoveAllReceivers();
	
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
	
	// Maps an OSC Address to a list of Objects that implement IOSCT_Router
	// FNames faster for lookups compared to FStrings for keys.
    TMap<FName, TArray<FOSCT_ReceiverLink>> ReceiverMap;
    TMap<FName, TArray<FOSCT_SenderLink>> SenderMap;

    // The function bound to OSCT_Server->OnOscMessageReceived
    UFUNCTION()
    void RouteMessage(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);
	
	UFUNCTION()
	static bool CheckIfSettled(const FOSCT_ReceiverLink& Link);
	
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