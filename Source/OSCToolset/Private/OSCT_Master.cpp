#include "OSCT_Master.h"

#include "OSCManager.h"
#include "OSCT_Modules.h"
#include "Interfaces/OSCT_Router.h"
#include "OSCToolsetLog.h"
#include "Functions/OSCT_Functions.h"
#include "Functions/OSCT_Parsing.h"

#include "OSCT_Settings.h"
#include "UI/SOSCT_Menu.h"
#include "Widgets/SWeakWidget.h"

#include "AssetRegistry/AssetData.h"

#include "SocketSubsystem.h"
#include "IPAddress.h"

#include "Engine/World.h" // Required for UWorld
#include "Engine/Engine.h" //Required for GEngine
#include "Engine/GameViewportClient.h" // Required for GameViewport
#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h" // Required for APlayerController


void UOSCT_Master::AddReceiver(FOSCT_Receiver Receiver, UObject* Owner)
{
    if (!Owner) return;
    
    if (UOSCT_Functions::FormatAddress(
        Receiver.Role,
        Receiver.ModuleType,
        Receiver.Pack,
        Receiver.Address,
        Receiver.FormattedAddress))
    {
        FName AddressKey = FName(*Receiver.FormattedAddress);
        if (!Owner->Implements<UOSCT_Router>())
        {
            UE_LOG(OSCToolset, Warning, TEXT("%s needs to implement OSCT_Router interface. Trying to receive address: %s"), 
                *Owner->GetName(), *Receiver.FormattedAddress);
            return;
        }

        switch (Receiver.ModuleType)
        {
        case EOSCT_ModuleType::EVENT:
            if (Receiver.Pack) AddReceiverLink(EventPackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(EventLinks, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::FLOAT:
            if (Receiver.Pack) AddReceiverLink(FloatPackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(FloatLinks, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::VEC2:
            if (Receiver.Pack) AddReceiverLink(Vec2PackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(Vec2Links, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::VEC3:
            if (Receiver.Pack) AddReceiverLink(Vec3PackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(Vec3Links, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::ROTATION:
            if (Receiver.Pack) AddReceiverLink(RotationPackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(RotationLinks, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::COLOR:
            if (Receiver.Pack) AddReceiverLink(ColorPackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(ColorLinks, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::TRANSFORM:
            UE_LOG(OSCToolset, Warning, TEXT("Adding a transform ?"));
            if (Receiver.Pack) AddReceiverLink(TransformPackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(TransformLinks, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::STRING:
            if (Receiver.Pack) AddReceiverLink(StringPackLinks, AddressKey, Receiver, Owner);
            else               AddReceiverLink(StringLinks, AddressKey, Receiver, Owner);
            break;
        case EOSCT_ModuleType::NOTE:
            AddReceiverLink(NoteLinks, AddressKey, Receiver, Owner);
            break;
        default:
            break;
        }
        IOSCT_Router::Execute_OnReceiverAdded(Owner, Receiver);
        UOSCT_Functions::SendReceiverStateUpdate(OSCT_Client, Receiver, Owner, true);
        
        UE_LOG(OSCToolset, Log, TEXT("Added OSCT %s: %s to Owner: %s"), 
            *UOSCT_Functions::GetEnumString(Receiver.Role), *Receiver.FormattedAddress, *Owner->GetName());    
    }
    else
    {
        // ERROR: This is where you log it!
        UE_LOG(OSCToolset, Error, TEXT("OSCT_Master: Failed to add module from %s. Address or Prefix is missing."), 
            *Owner->GetName());
    }
}

void UOSCT_Master::AddManyReceivers(TArray<FOSCT_Receiver> Receivers, UObject* Owner)
{
    for (const FOSCT_Receiver Module : Receivers)
    {
        AddReceiver(Module, Owner);
    }
}

void UOSCT_Master::AddReceiversFromDataTable(UDataTable* InTable, UObject* Owner)
{
    if (!InTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("OSCT_Master: Register attempted with null table."));
        return;
    }

    // // Ensure the table uses our struct
    // if (InTable->RowStruct->IsChildOf(FOSCT_Receiver::StaticStruct()) || 
    //     InTable->RowStruct->IsChildOf(FOSCT_Module::StaticStruct()))
    // {
    //     static const FString ContextString(TEXT("OSC Receiver Ingestion"));
    //     TArray<FOSCT_Receiver*> Rows;
    //     InTable->GetAllRows<FOSCT_Receiver>(ContextString, Rows);
    //
    //     // for (FOSCT_Receiver* RowData : Rows)
    //     // {
    //     //     if (RowData)
    //     //     {
    //     //         // Create the link
    //     //         FOSCT_ReceiverLink NewLink;
    //     //         
    //     //         // Copy the data from the table
    //     //         NewLink.ReceiverData = *RowData;
    //     //         
    //     //         // CRITICAL: Assign the owner so filters work!
    //     //         NewLink.ReceiverData.Owner = Owner;
    //     //         
    //     //         // Force the cache to build now so it's ready for the first OSC message
    //     //         // NewLink.ReceiverData.RefreshCache();
    //     //
    //     //         // Add to your Master's active list
    //     //         ReceiverMap.FindOrAdd(FormattedAddress).AddUnique(NewLink);
    //     //         
    //     //         UE_LOG(LogTemp, Log, TEXT("Registered OSC Receiver: %s"), *NewLink.ReceiverData.GetFormattedAddress());
    //     //     }
    //     // }
    // }
}

void UOSCT_Master::RemoveReceiver(FOSCT_Receiver Module, UObject* Owner)
{
    if (!Owner) return;
    if (UOSCT_Functions::FormatAddress(
        Module.Role,
        Module.ModuleType,
        Module.Pack,
        Module.Address,
        Module.FormattedAddress))
    {
        FName AddressKey = FName(*Module.FormattedAddress);
        if (const EOSCT_RouteType* RouteTypePtr = AddressToType.Find(AddressKey))
        {
            switch (*RouteTypePtr)
            {
            case EOSCT_RouteType::EVENT:
                RemoveReceiverLink(EventLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::EVENT_PACK:
                RemoveReceiverLink(EventPackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::FLOAT:
                RemoveReceiverLink(FloatLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::FLOAT_PACK:
                RemoveReceiverLink(FloatPackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::VEC2:
                RemoveReceiverLink(Vec2Links, AddressToType, AddressKey, Owner);
                break;;
            case EOSCT_RouteType::VEC2_PACK:
                RemoveReceiverLink(Vec2PackLinks, AddressToType, AddressKey, Owner);
                break;;
            case EOSCT_RouteType::VEC3:
                RemoveReceiverLink(Vec3Links, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::VEC3_PACK:
                RemoveReceiverLink(Vec3PackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::ROTATION:
                RemoveReceiverLink(RotationLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::ROTATION_PACK:
                RemoveReceiverLink(RotationPackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::COLOR:
                RemoveReceiverLink(ColorLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::COLOR_PACK:
                RemoveReceiverLink(ColorPackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::TRANSFORM:
                RemoveReceiverLink(TransformLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::TRANSFORM_PACK:
                RemoveReceiverLink(TransformPackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::STRING:
                RemoveReceiverLink(StringLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::STRING_PACK:
                RemoveReceiverLink(StringPackLinks, AddressToType, AddressKey, Owner);
                break;
            case EOSCT_RouteType::NOTE:
                RemoveReceiverLink(NoteLinks, AddressToType, AddressKey, Owner);
                break;
            default:
                UE_LOG(OSCToolset, Warning, TEXT("Did not implement removing the receiver for this type: %s"), *Module.FormattedAddress);
                break;
            }
            //Maybe send the disconnected module state update?
            UE_LOG(OSCToolset, Log, TEXT("Unregistered OSCT Receiver: %s"), *Module.FormattedAddress);
        }
    }
}

void UOSCT_Master::RemoveAllReceivers()
{
    CleanupLinks();    
    UE_LOG(OSCToolset, Log, TEXT("Removed all receivers."));
}

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
        OSCT_Server->OnOscMessageReceived.AddDynamic(this, &UOSCT_Master::RouteMessage);
    }
    if (OSCT_Client != nullptr)
    {
        OnInitOSCT.Broadcast();
        UE_LOG(OSCToolset, Log, TEXT("Initialize OSC from OSCT_Master"));
    }
}

void UOSCT_Master::OnLevelChanged(const FString& LevelName)
{
    // RemoveAllReceivers(); //Make sure the listeners are gone before a new level is loaded.
    UE_LOG(OSCToolset, Log, TEXT("OnLevelChanged: %s"), *LevelName);
    SendOSCTBaseMessage(OSCT_OnLevelChanged_addr);
}

void UOSCT_Master::CleanupLinks()
{
    EventLinks.Empty();
    EventPackLinks.Empty();
    FloatLinks.Empty();
    FloatPackLinks.Empty();
    Vec2Links.Empty();
    Vec2PackLinks.Empty();
    Vec3Links.Empty();
    Vec3PackLinks.Empty();
    RotationLinks.Empty();
    RotationPackLinks.Empty();
    ColorLinks.Empty();
    ColorPackLinks.Empty();
    TransformLinks.Empty();
    TransformPackLinks.Empty();
    StringLinks.Empty();
    StringPackLinks.Empty();
    NoteLinks.Empty();
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


void UOSCT_Master::LogSettings()
{
    UE_LOG(OSCToolset, Log, TEXT("Server Address: %s | Server Port: %d | Client Address: %s | Client Port: %d"), *SetLocalIPAddress(Settings->ServerAddress, true), Settings->ServerPort, *SetLocalIPAddress(Settings->ClientAddress, true), Settings->ClientPort);
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
                    reinit_OSCT_Master();
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
    CleanupLinks();
    FCoreUObjectDelegates::PreLoadMap.RemoveAll(this); //Cleanup On Level Loaded Static Delegate.
}

void UOSCT_Master::shutdown_OSCT_Master()
{
    // RemoveAllReceivers();
    
    OnShutdownOSCT.Broadcast();

    if (IsValid(OSCT_Client))
    {
        FOSCAddress Addr = UOSCManager::ConvertStringToOSCAddress(OSCT_Shutdown_addr);
        
        // Always check if the address is valid before sending
        if (!Addr.GetFullPath().IsEmpty())
        {
            FOSCMessage Msg;
            UOSCManager::SetOSCMessageAddress(Msg, Addr);
            
            UOSCManager::AddString(Msg, TEXT("OSCT Shutdown"));

            OSCT_Client->SendOSCMessage(Msg);
        }

        // Do NOT call ConditionalBeginDestroy(). 
        // Just null it; the Subsystem's death will handle the rest.
        OSCT_Client = nullptr;
    }

    if (IsValid(OSCT_Server))
    {
        // Stop listening immediately
        OSCT_Server->OnOscMessageReceived.RemoveAll(this);
        OSCT_Server = nullptr;
    }

    UE_LOG(OSCToolset, Log, TEXT("OSCT_MasterSubsystem Deinitialized"));
}

void UOSCT_Master::reinit_OSCT_Master()
{
    UE_LOG(OSCToolset, Log, TEXT("Start Re-Init the OSCT Master."));
    UE_LOG(OSCToolset, Log, TEXT("Total unique receivers in Map: %d"), ReceiverMap.Num());
    shutdown_OSCT_Master(); //First shutdown and clear
    init_OSCT_Master(); //Then init the OSCT Master again
    
    ReSendAllReceiversStateUpdate(); //For manual reset, we need to call the update.
}

void UOSCT_Master::ReSendAllReceiversStateUpdate()
{
    //Resend the module connected for each added receiver:
    for (auto& Pair : ReceiverMap)
    {
        // Pair.Key is the FName
        // Pair.Value is the TArray<FOSCT_ReceiverLink>
        for (FOSCT_ReceiverLink& Link : Pair.Value)
        {
            // Example: UE_LOG(LogTemp, Log, TEXT("Key: %s | Link Data: %s"), *Pair.Key.ToString(), *Link.SomeProperty);
            UOSCT_Functions::SendReceiverStateUpdate(OSCT_Client, Link.Data, Link.GetOwner(), true);
        }
    }

}

bool UOSCT_Master::SetupSender(FOSCT_Sender& Sender, const EOSCT_ModuleType ModuleType, UObject* Owner)
{
    if (Sender.FormattedAddress.IsEmpty())
    {
        UOSCT_Functions::FormatAddress(
            Sender.Role, 
            ModuleType,
            false,
            Sender.Address,
            Sender.FormattedAddress);
        
        Sender.CachedFullAddress = UOSCManager::ConvertStringToOSCAddress(Sender.FormattedAddress);
        Sender.Type = UOSCT_Functions::ConvertModuleTypeToSenderType(ModuleType);
        UE_LOG(OSCToolset, Log, TEXT("Added a new sender? %s"), *Sender.FormattedAddress)
        UOSCT_Functions::SendSenderStateUpdate(OSCT_Client, Sender, Owner, true);
        return true;
    } 
    return false;
}

void UOSCT_Master::Send_Event(FOSCT_Sender& Sender, UObject* Owner)
{
    SetupSender(Sender, EOSCT_ModuleType::EVENT, Owner);
    if (OSCT_Client && !Sender.CachedFullAddress.GetFullPath().IsEmpty())
    {
        float GameTime = GetWorld()->GetRealTimeSeconds();
        FOSCMessage Msg;
        UOSCManager::AddFloat(Msg, GameTime);
        UOSCManager::SetOSCMessageAddress(Msg, Sender.CachedFullAddress);
        OSCT_Client->SendOSCMessage(Msg);
    }
}

void UOSCT_Master::Send_Float(FOSCT_Sender& Sender, const float Value, UObject* Owner)
{
    SetupSender(Sender, EOSCT_ModuleType::FLOAT, Owner);
    if (OSCT_Client && !Sender.CachedFullAddress.GetFullPath().IsEmpty())
    {
        FOSCMessage Msg;
        UOSCManager::AddFloat(Msg, Value);
        UOSCManager::SetOSCMessageAddress(Msg, Sender.CachedFullAddress);
        OSCT_Client->SendOSCMessage(Msg);
    }
}

void UOSCT_Master::Send_String(FOSCT_Sender& Sender, const FString& Value, UObject* Owner)
{
    SetupSender(Sender, EOSCT_ModuleType::STRING, Owner);
    if (OSCT_Client && !Sender.CachedFullAddress.GetFullPath().IsEmpty())
    {
        FOSCMessage Msg;
        UOSCManager::AddString(Msg, Value);
        UOSCManager::SetOSCMessageAddress(Msg, Sender.CachedFullAddress);
        OSCT_Client->SendOSCMessage(Msg);
    }
}

bool UOSCT_Master::HandleCommands(const FOSCMessage& InMessage)
{
    FOSCAddress Addr = UOSCManager::GetOSCMessageAddress(InMessage);
    FString Path = UOSCManager::GetOSCAddressFullPath(Addr);
    
    // Check if this message is actually a command for us
    FString TargetCmdPath = OSCT_Base_addr + "R/CMD";
    if (!Path.Equals(TargetCmdPath))
    {
        return false; // Not a command message
    }

    // It is a command! Proceed with execution
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        FString Cmd = "";
        if (UOSCManager::GetString(InMessage, 0, Cmd))
        {
            UE_LOG(OSCToolset, Log, TEXT("Executing Command: %s"), *Cmd);
            PC->ConsoleCommand(Cmd, true);
        }
    }
    return true; // Message handled
}


void UOSCT_Master::RouteMessage(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{

    FString Address = UOSCManager::GetOSCAddressFullPath(UOSCManager::GetOSCMessageAddress(InMessage));
    FName AddressKey = FName(*Address);
    
    if (const EOSCT_RouteType* RouteTypePtr = AddressToType.Find(AddressKey))
    {
        switch (*RouteTypePtr)
        {
        case EOSCT_RouteType::FLOAT:
        {
            float Val;
            if (UOSCT_Parsing::TryGetFloat(InMessage, Val))
            {
                if (auto* Links = UpdateAndPrune(FloatLinks, AddressToType, AddressKey, Val))
                {
                    for (const FOSCT_FloatLink& Link : *Links)
                    {
                        IOSCT_Router::Execute_GET_Float(Link.Owner.Get(), Link.Data, Val);
                        if (!Link.bNeedsInterpolation)
                        { 
                            //First message to Tick events on the Router.
                            IOSCT_Router::Execute_GET_Float_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                        }
                    }
                }
            }
        break;
        }
        case EOSCT_RouteType::EVENT:
        {
            if (auto* Links = UpdateAndPrune(EventLinks, AddressToType, AddressKey, false))
            {
                for (const FOSCT_EventLink& Link : *Links)
                {
                    IOSCT_Router::Execute_GET_Event(Link.GetOwner(), Link.Data);
                }
            }
        break;
        }
        case EOSCT_RouteType::VEC2:
        {
            FVector2D Val;
            if (UOSCT_Parsing::TryGetVector2(InMessage, Val))
            {
                if (auto* Links = UpdateAndPrune(Vec2Links, AddressToType, AddressKey, Val))
                {
                    for (const FOSCT_Vector2Link& Link : *Links)
                    {
                        IOSCT_Router::Execute_GET_Vector2(Link.GetOwner(), Link.Data, Val);
                    }
                }
            }
            break;
        }
        case EOSCT_RouteType::VEC3:
            {
                FVector Val;
                if (UOSCT_Parsing::TryGetVector3(InMessage, Val))
                {
                    if (auto* Links = UpdateAndPrune(Vec3Links, AddressToType, AddressKey, Val))
                    {
                        for (const FOSCT_Vector3Link& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Vector3(Link.GetOwner(), Link.Data, Val);
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::ROTATION:
            {
                FRotator Val;
                if (UOSCT_Parsing::TryGetRotation(InMessage, Val))
                {
                    if (auto* Links = UpdateAndPrune(RotationLinks, AddressToType, AddressKey, Val))
                    {
                        for (const FOSCT_RotationLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Rotation(Link.GetOwner(), Link.Data, Val);
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::COLOR:
            {
                FLinearColor Val;
                if (UOSCT_Parsing::TryGetColor(InMessage, Val))
                {
                    if (auto* Links = UpdateAndPrune(ColorLinks, AddressToType, AddressKey, Val))
                    {
                        for (const FOSCT_ColorLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Color(Link.GetOwner(), Link.Data, Val);
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::TRANSFORM:
            {
                FTransform Val;
                if (UOSCT_Parsing::TryGetTransform(InMessage, Val))
                {
                    if (auto* Links = UpdateAndPrune(TransformLinks, AddressToType, AddressKey, Val))
                    {
                        for (const FOSCT_TransformLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Transform(Link.GetOwner(), Link.Data, Val);
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::STRING:
            {
                FString Val;
                if (UOSCT_Parsing::TryGetString(InMessage, Val))
                {
                    if (auto* Links = UpdateAndPrune(StringLinks, AddressToType, AddressKey, Val))
                    {
                        for (const FOSCT_StringLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_String(Link.GetOwner(), Link.Data, Val);
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::NOTE:
            {
                FOSCT_Note Val;
                if (UOSCT_Parsing::TryGetNotes(InMessage, Val))
                {
                    if (auto* Links = UpdateAndPrune(NoteLinks, AddressToType, AddressKey, Val))
                    {
                        for (const FOSCT_NoteLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Notes(Link.GetOwner(), Link.Data, Val);
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::FLOAT_PACK:
            {
                TMap<FString, float> Map;
                if (UOSCT_Parsing::TryGetFloatPack(InMessage, Map))
                {
                    if (auto * Links = UpdateAndPrune(FloatPackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_FloatPackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Float_Pack(Link.Owner.Get(), Link.Data, Map);
                            if (!Link.bNeedsInterpolation)
                            { 
                                //First message to Tick events on the Router.
                                IOSCT_Router::Execute_GET_Float_Pack_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                            }
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::EVENT_PACK:
            {
                TMap<FString, bool> Map;
                if (UOSCT_Parsing::TryGetEventPack(InMessage, Map))
                {
                    if (auto* Links = UpdateAndPrune(EventPackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_EventPackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Event_Pack(Link.Owner.Get(), Link.Data, Map);
                            
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::VEC2_PACK:
            {
                TMap<FString, FVector2D> Map;
                if (UOSCT_Parsing::TryGetVector2Pack(InMessage, Map))
                {
                    if (auto * Links = UpdateAndPrune(Vec2PackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_Vector2PackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Vector2_Pack(Link.Owner.Get(), Link.Data, Map);
                            if (!Link.bNeedsInterpolation)
                            { 
                                //First message to Tick events on the Router.
                                IOSCT_Router::Execute_GET_Vector2_Pack_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                            }
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::VEC3_PACK:
        {
            TMap<FString, FVector> Map;
            if (UOSCT_Parsing::TryGetVector3Pack(InMessage, Map))
            {
                if (auto * Links = UpdateAndPrune(Vec3PackLinks, AddressToType, AddressKey, Map))
                {
                    for (const FOSCT_Vector3PackLink& Link : *Links)
                    {
                        IOSCT_Router::Execute_GET_Vector3_Pack(Link.Owner.Get(), Link.Data, Map);
                        if (!Link.bNeedsInterpolation)
                        { 
                            //First message to Tick events on the Router.
                            IOSCT_Router::Execute_GET_Vector3_Pack_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                        }
                    }
                }
            }
            break;
        }
        case EOSCT_RouteType::ROTATION_PACK:
            {
                TMap<FString, FRotator> Map;
                if (UOSCT_Parsing::TryGetRotationPack(InMessage, Map))
                {
                    if (auto * Links = UpdateAndPrune(RotationPackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_RotationPackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Rotation_Pack(Link.Owner.Get(), Link.Data, Map);
                            if (!Link.bNeedsInterpolation)
                            { 
                                //First message to Tick events on the Router.
                                IOSCT_Router::Execute_GET_Rotation_Pack_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                            }
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::COLOR_PACK:
            {
                TMap<FString, FLinearColor> Map;
                if (UOSCT_Parsing::TryGetColorPack(InMessage, Map))
                {
                    if (auto * Links = UpdateAndPrune(ColorPackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_ColorPackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Color_Pack(Link.Owner.Get(), Link.Data, Map);
                            if (!Link.bNeedsInterpolation)
                            { 
                                //First message to Tick events on the Router.
                                IOSCT_Router::Execute_GET_Color_Pack_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                            }
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::TRANSFORM_PACK:
            {
                TMap<FString, FTransform> Map;
                
                if (UOSCT_Parsing::TryGetTransformPack(InMessage, Map))
                {
                    if (auto * Links = UpdateAndPrune(TransformPackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_TransformPackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_Transform_Pack(Link.Owner.Get(), Link.Data, Map);
                            if (!Link.bNeedsInterpolation)
                            { 
                                //First message to Tick events on the Router.
                                IOSCT_Router::Execute_GET_Transform_Pack_Tick(Link.Owner.Get(), Link.Data, Link.CurrentValue);
                            }
                        }
                    }
                }
                break;
            }
        case EOSCT_RouteType::STRING_PACK:
            {
                TArray<FString> Map;
                if (UOSCT_Parsing::TryGetStringPack(InMessage, Map))
                {
                    if (auto * Links = UpdateAndPrune(StringPackLinks, AddressToType, AddressKey, Map))
                    {
                        for (const FOSCT_StringPackLink& Link : *Links)
                        {
                            IOSCT_Router::Execute_GET_String_Pack(Link.Owner.Get(), Link.Data, Map);
                        }
                    }
                }
                break;
            }
        default:
            UE_LOG(OSCToolset, Warning, TEXT("Received a message not well formatted from a Receiver: %s"), *Address);
            break;
        }
    }
    // Try to handle as a command last
    HandleCommands(InMessage);
}

void UOSCT_Master::Tick(float DeltaTime)
{
    // Important: Only tick if the game is actually running/unpaused
    UWorld* World = GetWorld();
    if (!World || World->IsPaused()) return;
    if (TickableAddresses.Num() == 0) return; //If no active addresses.
    
    for (auto It = TickableAddresses.CreateIterator(); It; ++It)
    {
        FName Address = *It;
        bool bAnyLinkStillMoving = false;
        // Float
        ProcessActiveLinksTick<FOSCT_FloatLink>(FloatLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_FloatLink& Link, const float& Val) {
            IOSCT_Router::Execute_GET_Float_Tick(Obj, Link.Data, Val);
        });
        ProcessActiveLinksTick<FOSCT_FloatPackLink>(FloatPackLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_FloatPackLink& Link, const TMap < FString, float >& Val) {
            IOSCT_Router::Execute_GET_Float_Pack_Tick(Obj, Link.Data, Val);
        });
        //Vector 2
        ProcessActiveLinksTick<FOSCT_Vector2Link>(Vec2Links, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_Vector2Link& Link, const FVector2D& Val) {
            IOSCT_Router::Execute_GET_Vector2_Tick(Obj, Link.Data, Val);
        });
        ProcessActiveLinksTick<FOSCT_Vector2PackLink>(Vec2PackLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_Vector2PackLink& Link, const TMap < FString, FVector2D >& Val) {
            IOSCT_Router::Execute_GET_Vector2_Pack_Tick(Obj, Link.Data, Val);
        });
        //Vector 3
        ProcessActiveLinksTick<FOSCT_Vector3Link>(Vec3Links, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_Vector3Link& Link, const FVector& Val) {
            IOSCT_Router::Execute_GET_Vector3_Tick(Obj, Link.Data, Val);
        });
        ProcessActiveLinksTick<FOSCT_Vector3PackLink>(Vec3PackLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_Vector3PackLink& Link, const TMap < FString, FVector >& Val) {
            IOSCT_Router::Execute_GET_Vector3_Pack_Tick(Obj, Link.Data, Val);
        });
        // //Rotation
        ProcessActiveLinksTick<FOSCT_RotationLink>(RotationLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_RotationLink& Link, const FRotator& Val) {
            IOSCT_Router::Execute_GET_Rotation_Tick(Obj, Link.Data, Val);
        });
        ProcessActiveLinksTick<FOSCT_RotationPackLink>(RotationPackLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_RotationPackLink& Link, const TMap < FString, FRotator >& Val) {
            IOSCT_Router::Execute_GET_Rotation_Pack_Tick(Obj, Link.Data, Val);
        });
        //Color
        ProcessActiveLinksTick<FOSCT_ColorLink>(ColorLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_ColorLink& Link, const FLinearColor& Val) {
            IOSCT_Router::Execute_GET_Color_Tick(Obj, Link.Data, Val);
        });
        ProcessActiveLinksTick<FOSCT_ColorPackLink>(ColorPackLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_ColorPackLink& Link, const TMap < FString, FLinearColor >& Val) {
            IOSCT_Router::Execute_GET_Color_Pack_Tick(Obj, Link.Data, Val);
        });
        //Transform
        ProcessActiveLinksTick<FOSCT_TransformLink>(TransformLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_TransformLink& Link, const FTransform& Val) {
            IOSCT_Router::Execute_GET_Transform_Tick(Obj, Link.Data, Val);
        });
        ProcessActiveLinksTick<FOSCT_TransformPackLink>(TransformPackLinks, Address, DeltaTime, bAnyLinkStillMoving, 
        [](UObject* Obj, const FOSCT_TransformPackLink& Link, const TMap < FString, FTransform >& Val) {
            IOSCT_Router::Execute_GET_Transform_Pack_Tick(Obj, Link.Data, Val);
        });
        if (!bAnyLinkStillMoving) It.RemoveCurrent();
    }
}