#include "OSCT_Master.h"
#include "OSCManager.h"

#include "OSCT_Settings.h"
#include "UI/SOSCT_Menu.h"
#include "Engine/Engine.h"
#include "Widgets/SWeakWidget.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"

#include "Engine/World.h" // Required for UWorld
#include "Engine/Engine.h" //Required for GEngine
#include "Engine/GameViewportClient.h" // Required for GameViewport
#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h" // Required for APlayerController

#include "Kismet/KismetStringLibrary.h"

void UOSCT_Master::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Logic to initialize OSC communication or other services
    UE_LOG(OSCToolset, Log, TEXT("OSCT_MasterSubsystem Initialized"));
    IPV4 = GetLocalIPAddress();

    init_OSCT_Master();

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UOSCT_Master::OnLevelChanged);
}

void UOSCT_Master::init_OSCT_Master()
{
    Settings = GetMutableDefault<UOSCT_Settings>();

    if (Settings)
    {
        // Successfully loaded settings, now you can use them to initialize your OSC server/client
        UE_LOG(OSCToolset, Log, TEXT("OSCT Settings Loaded Successfully"));
        InitializeOSC();
        LogSettings();
    }
}

void UOSCT_Master::InitializeOSC()
{
    FString Server_addr = SetLocalIPAddress(Settings->ServerAddress);
    int Server_port= Settings->ServerPort;
    bool Multicast_loopback = Settings->MulticastLoopback;

    FString Client_addr= SetLocalIPAddress(Settings->ClientAddress);
    int Client_port = Settings->ClientPort;

    OSCT_Server = UOSCManager::CreateOSCServer(Server_addr, Server_port, Multicast_loopback, true, "OSCT_Server", this);
    OSCT_Client = UOSCManager::CreateOSCClient(Client_addr, Client_port, "OSCT_Client", this);
    if (OSCT_Server != nullptr)
    {
        SendOSCTBaseMessage(OSCT_Init_addr);
        UE_LOG(OSCToolset, Log, TEXT("Send OSCT Master Init message"));
        OSCT_Server->OnOscMessageReceived.AddDynamic(this, &UOSCT_Master::SetCommands);
    }
    if (OSCT_Client != nullptr)
    {
        OnInitOSCT.Broadcast();
        UE_LOG(OSCToolset, Log, TEXT("Initialize OSC from OSCT_Master"));
    }
}

void UOSCT_Master::OnLevelChanged(const FString& LevelName)
{
    UE_LOG(OSCToolset, Log, TEXT("OnLevelChanged: %s"), *LevelName);
    SendOSCTBaseMessage(OSCT_OnLevelChanged_addr);
}

void UOSCT_Master::SendOSCTBaseMessage(FString Message)
{
    if (OSCT_Client != nullptr) {
        FOSCAddress addr = UOSCManager::ConvertStringToOSCAddress(Message);
        if (addr.GetFullPath().IsEmpty()) {
            UE_LOG(OSCToolset, Error, TEXT("Invalid OSC Address created from message: %s"), *Message);
            return;
        }

        FOSCMessage msg;
        msg = UOSCManager::SetOSCMessageAddress(msg, addr);
        OSCT_Client->SendOSCMessage(UOSCManager::SetOSCMessageAddress(msg, addr));
    } 
    else {
        UE_LOG(OSCToolset, Error, TEXT(" OSCT_Client not in memory (nullptr). Can't send an OSCT Base Message: %s"), *Message);
    }
}

FString UOSCT_Master::GetLocalIPAddress()
{
    bool bCanBind = false;
    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
    if (Addr->IsValid())
    {
        return Addr->ToString(false); // Returns the IP address without the port
    }
    return FString();
}

FString UOSCT_Master::SetLocalIPAddress(FString InAddress, bool Log)
{
    if (InAddress == "localhost" || InAddress == "127.0.0.1")
    {
        if (IPV4.IsEmpty())
        {
            // In case IPV4 was not found we use localhost
            if (Log) 
            {
                UE_LOG(OSCToolset, Error, TEXT("Address-> %s OSCT Master could not find an IPV4, using localhost (127.0.0.1)"), *InAddress);
            }
            return "127.0.0.1";
        }
        else {
            // If the address that is set is localhost, use the local IPV4. A fix for 5.5
            if (Log)
            {
                UE_LOG(OSCToolset, Log, TEXT("Address-> %s OSCT Master using IPV4: %s as localhost (127.0.0.1)"), *InAddress, *IPV4);
            }
            return IPV4;
        }
    }
    else {
        // Otherwise use the selected address :^)
        return InAddress;
    }
}

void UOSCT_Master::SetCommands(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();

    FString cmd_addr = OSCT_Base_addr + "R/CMD";

    FString msg = UOSCManager::GetOSCAddressFullPath(UOSCManager::GetOSCMessageAddress(InMessage));

    if (UKismetStringLibrary::EqualEqual_StrStr(cmd_addr, msg))
    {
        //Once we checked if the Module Formatted address equals exactly to the incoming address, we can call GET_Message.
        if (PC)
        {
            FString cmd = "";
            UOSCManager::GetString(InMessage, 0, cmd);
            UE_LOG(LogTemp, Log, TEXT("Set command fixed rate: %s cmd: %s"), *msg, *cmd);
            PC->ConsoleCommand(cmd, true);
        }
    }

}

void UOSCT_Master::LogSettings()
{
    UE_LOG(OSCToolset, Log, TEXT("Server Address: %s | Server Port: %d | Client Addresss: %s | Client Port: %d"), *SetLocalIPAddress(Settings->ServerAddress, true), Settings->ServerPort, *SetLocalIPAddress(Settings->ClientAddress, true), Settings->ClientPort);
}

void UOSCT_Master::ToggleOSCTMenu()
{
    APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GWorld);

    if (GEngine) {
        if (!OSCTMenu.IsValid())
        {
            // Create the Slate widget if it doesn’t already exist
            SAssignNew(OSCTMenu, SOSCT_Menu);

            // Display it on screen
            GEngine->GameViewport->AddViewportWidgetContent(
                SNew(SWeakWidget).PossiblyNullContent(OSCTMenu.ToSharedRef()));
            
            // Bind the re-initialize event
            OSCTMenu->OnReInitOSCT.BindLambda([this]()
                {
                    // Handle re-initialization logic here
                    reinit_OSCT_Master(); // Replace with your actual re-init function
                });

            if (PlayerController) {
                FInputModeGameAndUI InputMode;
                InputMode.SetWidgetToFocus(OSCTMenu);
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = true;
            }
        
        }
        else
        {
            // Remove or toggle visibility by resetting it
            GEngine->GameViewport->RemoveViewportWidgetContent(OSCTMenu.ToSharedRef());
            OSCTMenu.Reset();

            // Revert input mode to game
            if (PlayerController)
            {
                FInputModeGameOnly InputMode;
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = false; // Hide the mouse cursor
            }
        }
    }
}

void UOSCT_Master::Deinitialize()
{
    shutdown_OSCT_Master();

    Super::Deinitialize();
    // Clean up any resources here

    FCoreUObjectDelegates::PreLoadMap.RemoveAll(this); //Cleanup On Level Loaded Static Delegate.

}

void UOSCT_Master::shutdown_OSCT_Master()
{
    OnShutdownOSCT.Broadcast();

    if (IsValid(OSCT_Client))
    {
        FOSCAddress addr = UOSCManager::ConvertStringToOSCAddress(OSCT_Shutdown_addr);
        if (addr.GetFullPath().IsEmpty())
        {
            UE_LOG(OSCToolset, Error, TEXT("Invalid OSC Address: %s"), *OSCT_Shutdown_addr);
            return;
        }

        FOSCMessage msg;
        msg = UOSCManager::SetOSCMessageAddress(msg, addr);

        OSCT_Client->SendOSCMessage(msg);
        OSCT_Client->ConditionalBeginDestroy(); // Proper cleanup
        OSCT_Client = nullptr;
    }
    else
    {
        UE_LOG(OSCToolset, Error, TEXT("OSCT_Client is already invalid."));
    }

    if (IsValid(OSCT_Server))
    {
        OSCT_Server->ConditionalBeginDestroy();
        OSCT_Server = nullptr;
    }

    UE_LOG(OSCToolset, Log, TEXT("OSCT_MasterSubsystem Deinitialized"));
}

void UOSCT_Master::reinit_OSCT_Master()
{
    UE_LOG(OSCToolset, Log, TEXT("Start Re-Init the OSCT Master."));

    shutdown_OSCT_Master(); //First shutdown and clear
    init_OSCT_Master(); //Then init the OSCT Master again
}
