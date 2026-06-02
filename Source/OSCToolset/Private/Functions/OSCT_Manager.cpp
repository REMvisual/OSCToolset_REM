// Fill out your copyright notice in the Description page of Project Settings.
#include "Functions/OSCT_Manager.h"
#include "Engine/GameInstance.h"
#include "OSCT_Master.h"
#include "OSCT_Modules.h"
#include "Functions/OSCT_Functions.h"

UOSCT_Master* UOSCT_Manager::GetMaster(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return nullptr;

    return GI->GetSubsystem<UOSCT_Master>();
}

void UOSCT_Manager::AddReceiver(UObject* WorldContextObject, FOSCT_Receiver Receiver, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->AddReceiver(Receiver, Owner);
	}
}

void UOSCT_Manager::AddManyReceivers(UObject* WorldContextObject, TArray<FOSCT_Receiver> Receivers, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->AddManyReceivers(Receivers, Owner);
	}
}

void UOSCT_Manager::AddReceiversFromDataTable(UObject* WorldContextObject, UDataTable* InTable, UObject* Owner, const FString& AddressFilter)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->AddReceiversFromDataTable(InTable, Owner, AddressFilter);
	}
}

void UOSCT_Manager::RemoveReceiver(UObject* WorldContextObject, FOSCT_Receiver Receiver, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->RemoveReceiver(Receiver, Owner);
	}

}

void UOSCT_Manager::RemoveAllReceivers(UObject* WorldContextObject)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->RemoveAllReceivers();
	}
}

void UOSCT_Manager::SendEvent(UObject* WorldContextObject, FOSCT_Sender& Sender, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Event(Sender, Owner);
	}
	
}

void UOSCT_Manager::SendBool(UObject* WorldContextObject, FOSCT_Sender& Sender, const bool Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Bool(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendFloat(UObject* WorldContextObject, FOSCT_Sender& Sender, const float Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Float(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendInteger(UObject* WorldContextObject, FOSCT_Sender& Sender, const int32 Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Integer(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendVector2(UObject* WorldContextObject, FOSCT_Sender& Sender, const FVector2D Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Vector2(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendVector3(UObject* WorldContextObject, FOSCT_Sender& Sender, const FVector Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Vector3(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendRotation(UObject* WorldContextObject, FOSCT_Sender& Sender, const FRotator Value,
	UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Rotation(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendColor(UObject* WorldContextObject, FOSCT_Sender& Sender, const FLinearColor Value,
	UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Color(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendTransform(UObject* WorldContextObject, FOSCT_Sender& Sender, const FTransform Value,
	UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Transform(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendString(UObject* WorldContextObject, FOSCT_Sender& Sender, const FString& Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_String(Sender, Value, Owner);
	}
}

//// LIVE VALUE GETTERS (read-only; registration is done separately via Add Receivers From Data Table)
bool UOSCT_Manager::GetOSCFloat(UObject* WorldContextObject, const FString& Address, float& OutValue, UObject* Owner)
{
	OutValue = 0.f;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::FLOAT, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetFloatValue(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCInteger(UObject* WorldContextObject, const FString& Address, int32& OutValue, UObject* Owner)
{
	OutValue = 0;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::INT, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetIntValue(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCVector2(UObject* WorldContextObject, const FString& Address, FVector2D& OutValue, UObject* Owner)
{
	OutValue = FVector2D::ZeroVector;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::VEC2, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetVector2Value(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCVector3(UObject* WorldContextObject, const FString& Address, FVector& OutValue, UObject* Owner)
{
	OutValue = FVector::ZeroVector;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::VEC3, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetVector3Value(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCRotation(UObject* WorldContextObject, const FString& Address, FRotator& OutValue, UObject* Owner)
{
	OutValue = FRotator::ZeroRotator;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::ROTATION, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetRotationValue(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCColor(UObject* WorldContextObject, const FString& Address, FLinearColor& OutValue, UObject* Owner)
{
	OutValue = FLinearColor::Black;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::COLOR, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetColorValue(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCTransform(UObject* WorldContextObject, const FString& Address, FTransform& OutValue, UObject* Owner)
{
	OutValue = FTransform::Identity;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::TRANSFORM, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetTransformValue(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCBool(UObject* WorldContextObject, const FString& Address, bool& OutValue, UObject* Owner)
{
	OutValue = false;
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::BOOL, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetBoolValue(Key, Owner, OutValue)) return true;
	return false;
}

bool UOSCT_Manager::GetOSCString(UObject* WorldContextObject, const FString& Address, FString& OutValue, UObject* Owner)
{
	OutValue = FString();
	UOSCT_Master* M = GetMaster(WorldContextObject);
	if (!M || !Owner) return false;

	FString Formatted;
	if (!UOSCT_Functions::FormatAddress(EOSCT_Role::RECEIVER, EOSCT_ModuleType::STRING, false, Address, Formatted)) return false;
	const FName Key(*Formatted);

	if (M->TryGetStringValue(Key, Owner, OutValue)) return true;
	return false;
}
