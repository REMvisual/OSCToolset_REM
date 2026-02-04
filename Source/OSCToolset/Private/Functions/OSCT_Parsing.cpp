// Fill out your copyright notice in the Description page of Project Settings.
#include "Functions/OSCT_Parsing.h"

#include "OSCManager.h"


bool UOSCT_Parsing::TryGetEvent(const FOSCMessage& InMessage, bool& OutValue)
{
	return true;
}

bool UOSCT_Parsing::TryGetFloat(const FOSCMessage& InMessage, float& OutValue)
{
	float X=0.0f;
	
	const bool bSuccess = UOSCManager::GetFloat(InMessage, 0, X);
	if (bSuccess)
	{
		OutValue = X;
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetFloatPack(const FOSCMessage& InMessage, TMap < FString, float >& OutMap)
{
	int32 Count = 0;
	// Index 0 is our Length header
	if (!UOSCManager::GetInt32(InMessage, 0, Count) || Count <= 0)
	{
		return false;
	}

	// Clear the map but keep the memory allocated for performance
	OutMap.Empty(Count);

	for (int32 i = 0; i < Count; ++i)
	{
		FString Key;
		float Value = 0.0f;

		// Key is at 1, 3, 5... | Value is at 2, 4, 6...
		bool bKeyOk = UOSCManager::GetString(InMessage, (i * 2) + 1, Key);
		bool bValOk = UOSCManager::GetFloat(InMessage, (i * 2) + 2, Value);

		if (bKeyOk && bValOk)
		{
			OutMap.Add(MoveTemp(Key), Value);
		}
	}

	// Return true only if we actually found data
	return OutMap.Num() > 0;
	
}

bool UOSCT_Parsing::TryGetVector2(const FOSCMessage& InMessage, FVector2D& OutValue)
{
	float X=0.0f, Y=0.0f;
	
	const bool bSuccess = UOSCManager::GetFloat(InMessage, 0, X) &&
		UOSCManager::GetFloat(InMessage, 1, Y);
	if (bSuccess)
	{
		OutValue = FVector2D(X, Y);
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetVector3(const FOSCMessage& InMessage, FVector& OutValue)
{
	float X=0.0f, Y=0.0f, Z=0.0f;

	// Aggregate the results. If any return false, the whole thing is false.
	const bool bSuccess = UOSCManager::GetFloat(InMessage, 0, X) &&
		UOSCManager::GetFloat(InMessage, 1, Y) &&
		UOSCManager::GetFloat(InMessage, 2, Z);
	
	if (bSuccess)
	{
		OutValue = FVector(X, Y, Z);
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetFloatPack(const FOSCMessage& InMessage, TMap<FString, FVector>& OutMap)
{
	int32 Count = 0;
	// Index 0: The "Length" header
	if (!UOSCManager::GetInt32(InMessage, 0, Count) || Count <= 0) return false;

	// Reuse memory: Clear the map but keep the buffer
	OutMap.Empty(Count);

	for (int32 i = 0; i < Count; ++i)
	{
		int32 KeyIndex = (i * 4) + 1;
        
		FString Key;
		float X=0.0f, Y=0.0f, Z=0.0f;
		
		bool bSuccess = UOSCManager::GetString(InMessage, KeyIndex, Key)&&
		UOSCManager::GetFloat(InMessage, KeyIndex + 1, X)&&
		UOSCManager::GetFloat(InMessage, KeyIndex + 2, Y)&&
		UOSCManager::GetFloat(InMessage, KeyIndex + 3, Z);

		if (bSuccess)
		{
			OutMap.Add(MoveTemp(Key), FVector(X, Y, Z));
		}
	}

	return OutMap.Num() > 0;
}

bool UOSCT_Parsing::TryGetRotation(const FOSCMessage& InMessage, FRotator& OutValue)
{
	float X=0.0f, Y=0.0f, Z=0.0f;
	
	// Aggregate the results. If any return false, the whole thing is false.
	const bool bSuccess = UOSCManager::GetFloat(InMessage, 0, X) &&
		UOSCManager::GetFloat(InMessage, 1, Y) &&
		UOSCManager::GetFloat(InMessage, 2, Z);
	
	if (bSuccess)
	{
		OutValue = FRotator(X, Y, Z);
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetColor(const FOSCMessage& InMessage, FLinearColor& OutValue)
{
	float R=0.0f, G=0.0f, B=0.0f, A=0.0f;
	
	const bool bSuccess = UOSCManager::GetFloat(InMessage, 0, R) &&
	UOSCManager::GetFloat(InMessage, 1, G) &&
	UOSCManager::GetFloat(InMessage, 2, B) &&
	UOSCManager::GetFloat(InMessage, 3, A);
	
	if (bSuccess)
	{
		OutValue = FLinearColor(R, G, B, A);
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetTransform(const FOSCMessage& InMessage, FTransform& OutValue)
{
	float TX=0.0f, TY=0.0f, TZ=0.0f;
	float RX=0.0f, RY=0.0f, RZ=0.0f;
	float SX=1.0f, SY=1.0f, SZ=1.0f;
	
	const bool bSuccess = UOSCManager::GetFloat(InMessage, 0, TX) &&
		UOSCManager::GetFloat(InMessage, 1, TY) &&
		UOSCManager::GetFloat(InMessage, 2, TZ) &&
		UOSCManager::GetFloat(InMessage, 3, RX) &&
		UOSCManager::GetFloat(InMessage, 4, RY) &&
		UOSCManager::GetFloat(InMessage, 5, RZ) &&
		UOSCManager::GetFloat(InMessage, 6, SX) &&
		UOSCManager::GetFloat(InMessage, 7, SY) &&
		UOSCManager::GetFloat(InMessage, 8, SZ);
		
	if (bSuccess)
	{
		OutValue.SetLocation(FVector(TX, TY, TZ));
		OutValue.SetRotation(FQuat(FRotator(RX, RY, RZ)));
		OutValue.SetScale3D(FVector(SX, SY, SZ));
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetString(const FOSCMessage& InMessage, FString& OutValue)
{
	FString Value;
	
	const bool bSuccess = UOSCManager::GetString(InMessage, 0, Value);
	if (bSuccess)
	{
		OutValue = Value;
		return true;
	}
	return false;
}

bool UOSCT_Parsing::TryGetNotes(const FOSCMessage& InMessage, FOSCT_Note& OutValue)
{
	int32 Pitch=0, Velocity=0, Voices=0;
	
	const bool bSuccess = UOSCManager::GetInt32(InMessage, 0, Pitch) &&
		UOSCManager::GetInt32(InMessage, 1, Velocity) &&
		UOSCManager::GetInt32(InMessage, 2, Voices);
	
	if (bSuccess)
	{
		OutValue = FOSCT_Note(Pitch, Velocity, Voices);
		return true;
	}
	return false;
}