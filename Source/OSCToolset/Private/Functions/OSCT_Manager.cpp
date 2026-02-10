// Fill out your copyright notice in the Description page of Project Settings.
#include "Functions/OSCT_Manager.h"
#include "OSCT_Master.h"

UOSCT_Master* UOSCT_Manager::GetMaster(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				return GI->GetSubsystem<UOSCT_Master>();
			}
		}
	}
	return nullptr;
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

void UOSCT_Manager::RemoveReceiver(UObject* WorldContextObject, FOSCT_Receiver Receiver, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->RemoveReceiver(Receiver, Owner);
	}

}

void UOSCT_Manager::SendEvent(UObject* WorldContextObject, FOSCT_Sender& Sender, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Event(Sender, Owner);
	}
	
}

void UOSCT_Manager::SendFloat(UObject* WorldContextObject, FOSCT_Sender& Sender, const float Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_Float(Sender, Value, Owner);
	}
}

void UOSCT_Manager::SendString(UObject* WorldContextObject, FOSCT_Sender& Sender, const FString& Value, UObject* Owner)
{
	if (UOSCT_Master* Master = GetMaster(WorldContextObject))
	{
		Master->Send_String(Sender, Value, Owner);
	}
}
