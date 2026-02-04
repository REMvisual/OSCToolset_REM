#include "OSCT_Master.h"

#include <rapidjson/document.h>

#include "OSCManager.h"

#include "Interfaces/OSCT_Router.h"
#include "OSCToolsetLog.h"
#include "Functions/OSCT_Functions.h"
#include "Functions/OSCT_Parsing.h"

#include "OSCT_Settings.h"
#include "UI/SOSCT_Menu.h"
#include "Engine/Engine.h"
#include "Widgets/SWeakWidget.h"

#include "AssetRegistry/AssetData.h"

#include "SocketSubsystem.h"
#include "IPAddress.h"

#include "Engine/World.h" // Required for UWorld
#include "Engine/Engine.h" //Required for GEngine
#include "Engine/GameViewportClient.h" // Required for GameViewport
#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h" // Required for APlayerController

#include "Kismet/KismetStringLibrary.h"


void UOSCT_Master::AddModule(FOSCT_Module Module, UObject* Owner)
{
    if (!Owner) return;
    
    if (UOSCT_Functions::FormatAddress(Module, Module.FormattedAddress))
    {
        if (!Owner->Implements<UOSCT_Router>())
        {
            UE_LOG(OSCToolset, Warning, TEXT("%s needs to implement OSCT_Router interface. Trying to receive address: %s"), *Owner->GetName(), *Module.FormattedAddress);
            return;
        }
        
        FName AddressKey = FName(*Module.FormattedAddress);
        
        FOSCT_ReceiverLink NewReceiverLink;
        NewReceiverLink.Data = Module;
        NewReceiverLink.Owner = Owner;
        
        ReceiverMap.FindOrAdd(AddressKey).AddUnique(NewReceiverLink);
        IOSCT_Router::Execute_OnRegisterOSCTModule(Owner, NewReceiverLink.Data);
        UOSCT_Functions::SendModuleStateUpdate(OSCT_Client, NewReceiverLink.Data, Owner, true);
        UE_LOG(OSCToolset, Log, TEXT("Added OSCT Receiver: %s to Owner: %s"), 
            *Module.FormattedAddress, *Owner->GetName());    
    }
    // if (!FormattedAddress.IsEmpty())
    // {
    //     if (!Owner->Implements<UOSCT_Router>())
    //     {
    //         UE_LOG(OSCToolset, Warning, TEXT("%s needs to implement OSCT_Router interface. Trying to receive address: %s"), *Owner->GetName(), *FormattedAddress);
    //         return;
    //     }
    //     
    //     FName AddressKey = FName(*FormattedAddress);
    //     
    //     FOSCT_ReceiverLink NewReceiverLink;
    //     NewReceiverLink.Data = Module;
    //     NewReceiverLink.Data.Owner = Owner;
    //     
    //     ReceiverMap.FindOrAdd(AddressKey).AddUnique(NewReceiverLink);
    //     IOSCT_Router::Execute_OnRegisterOSCTModule(Owner, NewReceiverLink.Data);
    //     UOSCT_Functions::SendModuleStateUpdate(OSCT_Client, NewReceiverLink.Data, Owner, true);
    //     UE_LOG(OSCToolset, Log, TEXT("Added OSCT Receiver: %s to Owner: %s"), 
    //         *FormattedAddress, *Owner->GetName());    
    //
    // }
    else
    {
        // ERROR: This is where you log it!
        UE_LOG(OSCToolset, Error, TEXT("OSCT_Master: Failed to add module from %s. Address or Prefix is missing."), *Owner->GetName());
    }
}

void UOSCT_Master::AddManyModules(TArray<FOSCT_Module> Modules, UObject* Owner)
{
    for (const FOSCT_Module Module : Modules)
    {
        AddModule(Module, Owner);
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


void UOSCT_Master::RemoveModule(const FOSCT_Module Module, UObject* Owner)
{
    if (!Owner) return;
    
    // const FString& FormattedAddress = Module.GetFormattedAddress();
    // FString Address;
    if (!Module.FormattedAddress.IsEmpty()) //Here the module should have the Formatted Address set.
    {
        FName AddressKey = FName(*Module.FormattedAddress);
        if (TArray<FOSCT_ReceiverLink>* Links = ReceiverMap.Find(AddressKey))
        {
            for (const FOSCT_ReceiverLink& Link : *Links)
            {
                if (Link.GetOwner() == Owner && Link.HasValidOwner())
                {
                    // Call the interface before actually deleting the link
                    IOSCT_Router::Execute_OnRemovedOSCTModule(Link.GetOwner(), Module);
                }
            }
            // Just do the work. The predicate handles the "Search & Destroy".
            Links->RemoveAll([Owner](const FOSCT_ReceiverLink& L) {
                return L.GetOwner() == Owner || !L.HasValidOwner();
            });
            // If the list is empty, delete the map key.
            if (Links->Num() == 0)
            {
                ReceiverMap.Remove(AddressKey);
            }
            UE_LOG(OSCToolset, Log, TEXT("Removed OSCT Receiver: %s to Owner: %s"), *Module.FormattedAddress, *Owner->GetName());

        }
    }
}

void UOSCT_Master::RemoveAllReceivers()
{
    // CreateIterator allows us to modify the map while we loop through it
    for (auto MapIt = ReceiverMap.CreateIterator(); MapIt; ++MapIt)
    {
        TArray<FOSCT_ReceiverLink>& Links = MapIt.Value();

        // 1. Prune the array of any links where the Owner is now null
        Links.RemoveAll([](const FOSCT_ReceiverLink& L) {
            return !L.HasValidOwner();
        });

        // 2. If no one is listening to this address anymore, kill the Map Key
        if (Links.Num() == 0)
        {
            MapIt.RemoveCurrent();
        }
    }
    
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
        // OSCT_Server->OnOscMessageReceived.AddDynamic(this, &UOSCT_Master::SetCommands); //TODO-Pass this to the Route Message Function.
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
            
            // Add a reason or timestamp if you want!
            UOSCManager::AddString(Msg, TEXT("User_Exit"));

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

    shutdown_OSCT_Master(); //First shutdown and clear
    init_OSCT_Master(); //Then init the OSCT Master again
}

void UOSCT_Master::RouteMessage(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
    FString Address = UOSCManager::GetOSCAddressFullPath(UOSCManager::GetOSCMessageAddress(InMessage));
    FName AddressKey = FName(*Address);
    TArray<FOSCT_ReceiverLink>* ReceiverLinks = ReceiverMap.Find(AddressKey);
    if (!ReceiverLinks) return;
    
    for (FOSCT_ReceiverLink& Link : *ReceiverLinks)
    {
        if (Link.HasValidOwner())
        {   
            switch (Link.Data.ModuleType)
            {
            case EOSCT_ModuleType::EVENT:
                {
                    IOSCT_Router::Execute_GET_Event(Link.GetOwner(), Link.Data);
                    break;   
                }
            case EOSCT_ModuleType::FLOAT:
                {
                    float Value;
                    if (UOSCT_Parsing::TryGetFloat(InMessage, Value))
                    {
                        Link.TargetValue.X = Value;
                        //Link.bNeedsInterpolation = true;
                        IOSCT_Router::Execute_GET_Float(Link.GetOwner(), Link.Data, Value);
                    }
                    break;
                }
            case EOSCT_ModuleType::VEC2:
                {
                    FVector2D Value;
                    if (UOSCT_Parsing::TryGetVector2(InMessage, Value))
                    {
                        Link.TargetValue.X = Value.X;
                        Link.TargetValue.Y = Value.Y;
                        //Link.bNeedsInterpolation = true;
                        
                        IOSCT_Router::Execute_GET_Vector2(Link.GetOwner(), Link.Data, Value);
                    }
                    break;
                }
            case EOSCT_ModuleType::VEC3:
                {
                    FVector Value;
                    if (UOSCT_Parsing::TryGetVector3(InMessage, Value))
                    {
                        Link.TargetValue.X = Value.X;
                        Link.TargetValue.Y = Value.Y;
                        Link.TargetValue.Z = Value.Z;
                        //Link.bNeedsInterpolation = true;
                        
                        IOSCT_Router::Execute_GET_Vector3(Link.GetOwner(), Link.Data, Value);
                    }
                break;
                }
            case EOSCT_ModuleType::ROTATION:
                {
                    FRotator Value;
                    if (UOSCT_Parsing::TryGetRotation(InMessage, Value))
                    {
                        Link.TargetRotation.Pitch = Value.Pitch;
                        Link.TargetRotation.Roll = Value.Roll;
                        Link.TargetRotation.Yaw = Value.Yaw;
                        //Link.bNeedsInterpolation = true;
                        
                        IOSCT_Router::Execute_GET_Rotation(Link.GetOwner(), Link.Data, Value);
                    }
                break;
                }
            case EOSCT_ModuleType::COLOR:
                {
                    FLinearColor Value;
                    if (UOSCT_Parsing::TryGetColor(InMessage, Value))
                    {
                        Link.TargetColor.R = Value.R;
                        Link.TargetColor.G = Value.G;
                        Link.TargetColor.B = Value.B;
                        Link.TargetColor.A = Value.A;
                        //Link.bNeedsInterpolation = true;
                        
                        IOSCT_Router::Execute_GET_Color(Link.GetOwner(), Link.Data, Value);
                    }
                break;
                }
            case EOSCT_ModuleType::TRANSFORM:
                {
                    FTransform Value;
                    if (UOSCT_Parsing::TryGetTransform(InMessage, Value))
                    {
                        Link.TargetValue = Value.GetLocation();
                        Link.TargetRotation = Value.Rotator();
                        Link.TargetScale = Value.GetScale3D();
                        //Link.bNeedsInterpolation = true;
                        
                        IOSCT_Router::Execute_GET_Transform(Link.GetOwner(), Link.Data, Value);
                    }
                break;
                }
            case EOSCT_ModuleType::STRING:
                {
                    FString Value;
                    if (UOSCT_Parsing::TryGetString(InMessage, Value))
                    {
                        //Link.bNeedsInterpolation = false;
                        IOSCT_Router::Execute_GET_String(Link.GetOwner(), Link.Data, Value);
                    }
                break;
                }
            case EOSCT_ModuleType::NOTE:
                {
                    FOSCT_Note Value;
                    if (UOSCT_Parsing::TryGetNotes(InMessage, Value))
                    {
                        Link.bNeedsInterpolation = false;
                        IOSCT_Router::Execute_GET_Notes(Link.GetOwner(), Link.Data, Value);
                    }
                break;
                }
            default:
                {
                //Non OSCT formatted messages... Send through IOSCT_Router::Execute_OnOSCMessageReceived
                break;
                }
            }
            IOSCT_Router::Execute_OnOSCMessageReceived(Link.GetOwner(), Address, InMessage);
            if (!Link.bInitialized)
            {
                //First message link
                Link.CurrentValue = Link.TargetValue;
                Link.CurrentRotation   = Link.TargetRotation;
                Link.CurrentScale = Link.TargetScale;
                Link.CurrentColor = Link.TargetColor;
                
                Link.bInitialized = true;
                Link.bNeedsInterpolation = false; // Stay asleep
                
                IOSCT_Router::Execute_OnOSCTModuleInit(Link.GetOwner(), Link.Data);
                
                //For modules that need interpolation, we call a first event to send the base value
                IOSCT_Router::Execute_GET_Float_Tick(Link.GetOwner(), Link.Data, Link.CurrentValue.X);
                IOSCT_Router::Execute_GET_Vector2_Tick(Link.GetOwner(), Link.Data, FVector2D(Link.CurrentValue.X, Link.CurrentValue.Y));
                IOSCT_Router::Execute_GET_Vector3_Tick(Link.GetOwner(), Link.Data, Link.CurrentValue);
                IOSCT_Router::Execute_GET_Rotation_Tick(Link.GetOwner(), Link.Data, Link.CurrentRotation);
                IOSCT_Router::Execute_GET_Color_Tick(Link.GetOwner(), Link.Data, Link.CurrentColor);
                FTransform TransformFirstValue(
                    Link.CurrentRotation, 
                    Link.CurrentValue,    
                    Link.CurrentScale     
                );
                IOSCT_Router::Execute_GET_Transform_Tick(Link.GetOwner(), Link.Data, TransformFirstValue);
            } else
            {
                Link.bNeedsInterpolation = Link.Data.Tick.bEnable;
            }
        }
    }
}
void UOSCT_Master::Tick(float DeltaTime)
{
    // Important: Only tick if the game is actually running/unpaused
    UWorld* World = GetWorld();
    if (!World || World->IsPaused()) return;

    
    for (auto& Pair : ReceiverMap)
    {
        for (FOSCT_ReceiverLink& Link : Pair.Value)
        {
            if (Link.bNeedsInterpolation && Link.HasValidOwner())
            {
                const float Speed = Link.Data.Tick.InterpolationSpeed;
                // 1. Interpolation based on type.
                switch (Link.Data.ModuleType)
                {
                case EOSCT_ModuleType::FLOAT:
                    Link.CurrentValue = FMath::VInterpTo(Link.CurrentValue, Link.TargetValue, DeltaTime, Speed);
                    IOSCT_Router::Execute_GET_Float_Tick(Link.GetOwner(), Link.Data, Link.CurrentValue.X);
                    break;
                case EOSCT_ModuleType::VEC2:
                    Link.CurrentValue = FMath::VInterpTo(Link.CurrentValue, Link.TargetValue, DeltaTime, Speed);
                    IOSCT_Router::Execute_GET_Vector2_Tick(Link.GetOwner(), Link.Data, FVector2D(Link.CurrentValue.X, Link.CurrentValue.Y));
                case EOSCT_ModuleType::VEC3:
                    Link.CurrentValue = FMath::VInterpTo(Link.CurrentValue, Link.TargetValue, DeltaTime, Speed);
                    IOSCT_Router::Execute_GET_Vector3_Tick(Link.GetOwner(), Link.Data, Link.CurrentValue);
                    break;
                case EOSCT_ModuleType::ROTATION:
                    Link.CurrentRotation = FMath::RInterpTo(Link.CurrentRotation, Link.TargetRotation, DeltaTime, Speed);
                    IOSCT_Router::Execute_GET_Rotation_Tick(Link.GetOwner(), Link.Data, Link.CurrentRotation);
                    break;
                case EOSCT_ModuleType::COLOR:
                    Link.CurrentColor = FMath::CInterpTo(Link.CurrentColor, Link.TargetColor, DeltaTime, Speed);
                    IOSCT_Router::Execute_GET_Color_Tick(Link.GetOwner(), Link.Data, Link.CurrentColor);
                    break;

                case EOSCT_ModuleType::TRANSFORM:
                    {
                    Link.CurrentValue = FMath::VInterpTo(Link.CurrentValue, Link.TargetValue, DeltaTime, Speed);
                    Link.CurrentRotation   = FMath::RInterpTo(Link.CurrentRotation, Link.TargetRotation, DeltaTime, Speed);
                    Link.CurrentScale = FMath::VInterpTo(Link.CurrentScale, Link.TargetScale, DeltaTime, Speed);
                    FTransform CurrentTransform(
                        Link.CurrentRotation, 
                        Link.CurrentValue,    
                        Link.CurrentScale     
                    );
                    IOSCT_Router::Execute_GET_Transform_Tick(Link.GetOwner(), Link.Data, CurrentTransform);
                    break;
                    }
                default: break;
                }
                if (CheckIfSettled(Link))
                {
                    // Sleep interpolation if the value is not changing...
                    Link.bNeedsInterpolation = false;
                }
            }
        }
    }
}
bool UOSCT_Master::CheckIfSettled(const FOSCT_ReceiverLink& Link)
{
    switch (Link.Data.ModuleType)
    {
    case EOSCT_ModuleType::ROTATION:
        return Link.CurrentRotation.Equals(Link.TargetRotation, 0.01f);

    case EOSCT_ModuleType::COLOR:
        // 0.001 in float is practically invisible in 8-bit color
        return Link.CurrentColor.Equals(Link.TargetColor, 0.001f);

    case EOSCT_ModuleType::TRANSFORM:
        // A Transform is only settled if all three parts are settled
        return Link.CurrentValue.Equals(Link.TargetValue, 0.001f) &&
               Link.CurrentRotation.Equals(Link.TargetRotation, 0.01f) &&
               Link.CurrentScale.Equals(Link.TargetScale, 0.001f);
    default:
        // Handles FLOAT, VEC2, VEC3
        return Link.CurrentValue.Equals(Link.TargetValue, 0.001f);
    }
}