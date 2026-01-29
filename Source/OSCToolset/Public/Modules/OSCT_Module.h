// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "OSCT_ETypes.h"
#include "OSCT_Settings.h"

#include "OSCMessage.h"
#include "OSCServer.h"
#include "OSCClient.h"

#include "OSCT_Master.h"

#include "OSCT_Module.generated.h"


//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitOSCTModule);

UCLASS(Abstract)
class OSCTOOLSET_API UOSCT_Module : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOSCT_Module();

	// Delegates //
	UPROPERTY(BlueprintAssignable)
	FOnInitOSCTModule On_Init_OSCT_Module;

	// MODULE //
	UPROPERTY()
	const UOSCT_Settings* Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	bool isPack;

	UPROPERTY()
	EOSCT_Role_Type RoleType;

	UPROPERTY()
	EOSCT_Module_Type ModuleType;

	UPROPERTY()
	EOSCT_Sender_Type SenderType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset", meta=(ExposeOnSpawn="true"))
	FString Address;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "OSCToolset")
	FString FormattedAddress;

	UPROPERTY()
	UOSCT_Master* OSCT_Master;

	UPROPERTY()
	UOSCServer* OSCT_Server;

	UPROPERTY()
	UOSCClient* OSCT_Client;


	// DEBUG //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	bool Debug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	bool PrintOnScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	bool PrintOnLog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	bool DebugAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	FColor DebugColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	float DebugDuration;


	// DEBUG MODULE DATA //
	FString OSCTMName;
	FString OSCTMClass;
	FString OSCTMOwner;

	FString OSCTModuleDebugString; // A string that formats the name, class and the owner of the module.

	// FUNCTIONS //

	UFUNCTION()
	void FormatOSCTAddress();

	UFUNCTION()
	void SendConnectedOSCTModule();


protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void init_OSCT_Module();

	UFUNCTION()
	virtual void shutdown_OSCT_Module();

	void OSCTDebugOSCMessage(const FString MessageValues);

	bool is_address_valid(const FString InAddress);

	void SetDebugColor();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	template<typename TEnum>
	FString GetEnumValueAsString(TEnum InEnum);

	void OSCTModuleError(const FString Message);

};
