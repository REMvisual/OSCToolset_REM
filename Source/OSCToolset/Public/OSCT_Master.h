// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

#include "OSCServer.h"
#include "OSCClient.h"
#include "OSCToolsetLog.h"

#include "OSCT_Settings.h"
#include "OSCT_ETypes.h"
#include "OSCT_Modules.h"
#include "Functions/OSCT_Functions.h"
#include "Interfaces/OSCT_Router.h"
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

	////// RECEIVERS
	////// RECEIVERS
	////// RECEIVERS
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	void AddReceiver(FOSCT_Receiver Receiver, UObject* Owner);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	void AddManyReceivers(TArray<FOSCT_Receiver> Receivers, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", 
		meta = (
			RequiredAssetDataTag = "RowStructure=/Script/OSCToolset.FOSCT_Receiver",
			DefaultToSelf = "Owner", 
			HidePin = "Owner"))
	void AddReceiversFromDataTable(UDataTable* InTable, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset", meta = (DefaultToSelf = "Owner", HidePin = "Owner"))
	void RemoveReceiver(FOSCT_Receiver Module, UObject* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset")
	void RemoveAllReceivers();
	
	////// SENDERS
	////// SENDERS
	////// SENDERS
	UFUNCTION()
	bool SetupSender(FOSCT_Sender& Sender, const EOSCT_ModuleType ModuleType, UObject* Owner);
	UFUNCTION()
	void Send_Event(UPARAM(ref) FOSCT_Sender& Sender, UObject* Owner );
	UFUNCTION()
	void Send_Float(UPARAM(ref) FOSCT_Sender& Sender,  const float Value, UObject* Owner);
	UFUNCTION()
	void Send_String(UPARAM(ref) FOSCT_Sender& Sender, const FString& Value, UObject* Owner);

	
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
	
	/// Modules
	TMap<FName, TArray<FOSCT_EventLink>> EventLinks;
	TMap<FName, TArray<FOSCT_EventPackLink>> EventPackLinks;
	
	TMap<FName, TArray<FOSCT_FloatLink>> FloatLinks;
	TMap<FName, TArray<FOSCT_FloatPackLink>> FloatPackLinks;
	
	TMap<FName, TArray<FOSCT_Vector2Link>> Vec2Links;
	TMap<FName, TArray<FOSCT_Vector2PackLink>> Vec2PackLinks;
	
	TMap<FName, TArray<FOSCT_Vector3Link>> Vec3Links;
	TMap<FName, TArray<FOSCT_Vector3PackLink>> Vec3PackLinks;
	
	TMap<FName, TArray<FOSCT_RotationLink>> RotationLinks;
	TMap<FName, TArray<FOSCT_RotationPackLink>> RotationPackLinks;
	
	TMap<FName, TArray<FOSCT_ColorLink>> ColorLinks;
	TMap<FName, TArray<FOSCT_ColorPackLink>> ColorPackLinks;
	
	TMap<FName, TArray<FOSCT_TransformLink>> TransformLinks;
	TMap<FName, TArray<FOSCT_TransformPackLink>> TransformPackLinks;
	
	TMap<FName, TArray<FOSCT_StringLink>> StringLinks;
	TMap<FName, TArray<FOSCT_StringPackLink>> StringPackLinks;
	
	TMap<FName, TArray<FOSCT_NoteLink>> NoteLinks;
	
	TMap<FName, EOSCT_RouteType> AddressToType;
	TSet<FName> TickableAddresses; //For checking during tick event.
	
	void CleanupLinks();
	
	template<typename TLink>
	void AddReceiverLink(TMap<FName, TArray<TLink>>& TargetMap, const FName& AddressKey, const FOSCT_Receiver& Receiver, UObject* Owner)
	{
		TLink NewLink;
		NewLink.Data = Receiver;
		NewLink.Owner = Owner;

		// Find the array (or create it) and add the link
		TargetMap.FindOrAdd(AddressKey).AddUnique(NewLink);
		AddressToType.FindOrAdd(AddressKey) = UOSCT_Functions::ConvertModuleTypeToRouteType(Receiver.ModuleType, Receiver.Pack);
	}

template<typename TLink, typename TValue>
TArray<TLink>* UpdateAndPrune(TMap<FName, TArray<TLink>>& TargetMap, 
							 TMap<FName, EOSCT_RouteType>& CacheMap, 
							 FName Key, const TValue& NewValue)
	{
		TArray<TLink>* LinkArray = TargetMap.Find(Key);
		if (!LinkArray) return nullptr;

		for (int32 i = LinkArray->Num() - 1; i >= 0; --i)
		{
			TLink& Link = (*LinkArray)[i];
			if (Link.Owner.IsValid())
			{
				Link.TargetValue = NewValue;
				
				// Handle Initialization State
				if (!Link.bInitialized)
				{
					Link.CurrentValue = NewValue;
					Link.bInitialized = true;
					Link.bNeedsInterpolation = false;
					IOSCT_Router::Execute_OnReceiverInit(Link.GetOwner(), Link.Data);
					UE_LOG(OSCToolset, Log, TEXT("Intialize key %s"), *Key.ToString());
				}
				else
				{
					// Toggle interpolation flag based on settings
					Link.bNeedsInterpolation = Link.Data.Tick.bEnable;
					//Check if it is a tickable address
					if (Link.Data.Tick.bEnable)
					{
						TickableAddresses.FindOrAdd(Key);
					}
				}
			}
			else
			{
				LinkArray->RemoveAtSwap(i);
			}
		}

		if (LinkArray->Num() == 0)
		{
			TargetMap.Remove(Key);
			CacheMap.Remove(Key);
			TickableAddresses.Remove(Key);
			return nullptr;
		}
		return LinkArray;
	}
	
	template<typename TLink>
	void ProcessActiveLinksTick(
		TMap<FName, 
		TArray<TLink>>& TargetMap, 
		FName Address, 
		float DeltaTime, 
		bool& bOutStillMoving, 
		TFunction<void(UObject*, const TLink&, const typename TLink::ValueType&)> ExecuteFunc)
	{
		if (TArray<TLink>* Links = TargetMap.Find(Address))
		{
			for (TLink& L : *Links)
			{
				if (L.bNeedsInterpolation && L.Owner.IsValid())
				{
					L.Interpolate(DeltaTime, L.Data.Tick.InterpolationSpeed, L.Tolerance);
                
					// We pass the whole Link 'L' so the interface has everything it needs
					ExecuteFunc(L.Owner.Get(), L, L.CurrentValue);

					if (L.IsSettled(L.Tolerance)) L.bNeedsInterpolation = false;
					else bOutStillMoving = true;
				}
			}
		}
	}
	
	
	template<typename TLink>
	bool RemoveReceiverLink(TMap<FName, TArray<TLink>>& TargetMap, 
							TMap<FName, EOSCT_RouteType>& CacheMap, 
							const FName& AddressKey, 
							UObject* Owner)
	{
		TArray<TLink>* Links = TargetMap.Find(AddressKey);
		if (!Links) return false;

		// 1. Optional: Call the "About to be removed" logic if needed
		// 2. Perform the Search & Destroy
		Links->RemoveAll([Owner](const TLink& L) {
			return !L.Owner.IsValid() || L.Owner.Get() == Owner;
		});

		// 3. Cleanup: If the array is now empty, wipe the key from both maps
		if (Links->Num() == 0)
		{
			TargetMap.Remove(AddressKey);
			CacheMap.Remove(AddressKey);
		}
    
		return true;
	}
	
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

	UFUNCTION()
	void ReSendAllReceiversStateUpdate();
	
    // The function bound to OSCT_Server->OnOscMessageReceived
    UFUNCTION()
    void RouteMessage(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort);
	
	// UFUNCTION()
	// static bool CheckIfSettled(const FOSCT_ReceiverLink& Link);
	
	FString OSCT_Base_addr = "/OSCT/";

	FString OSCT_Init_addr = OSCT_Base_addr + "init";

	FString OSCT_Shutdown_addr = OSCT_Base_addr + "shutdown";

	FString OSCT_OnLevelChanged_addr = OSCT_Base_addr + "level_changed";

	void InitializeOSC();

	void SendOSCTBaseMessage(FString Message);

	FString GetLocalIPAddress();

	FString SetLocalIPAddress(FString InAddress, bool Log = false);

	UFUNCTION()
	bool HandleCommands(const FOSCMessage& InMessage);

	void LogSettings();

	TSharedPtr<SOSCT_Menu> OSCTMenu;
};