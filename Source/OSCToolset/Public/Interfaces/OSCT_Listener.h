// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OSCT_Listener.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UOSCT_Listener : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OSCTOOLSET_API IOSCT_Listener
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Message received main event
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset")
	void OnOSCMessageReceived(const FString& Address, const FOSCMessage& Message);
	
	
	// Event
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset")
	void GET_Event(const FString& Address);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Packs")
	void GET_Event_Pack(const FString& Address, const TMap<FString, bool>& Map);
	
	// Float
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset")
	void GET_Float(const FString& Address, const float Value);
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Packs")
	void GET_Float_Pack(const FString& Address, const TMap<FString, float>& Map);
	
	//Vector 2
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset")
	void GET_Vector2(const FString& Address, const FVector2D& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Packs")
	void GET_Vector2_Pack(const FString& Address, const TMap<FString, FVector2D>& Map);

	//Vector 3
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset")
	void GET_Vector3(const FString& Address, const FVector& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Packs")
	void GET_Vector3_Pack(const FString& Address, const TMap<FString, FVector>& Map);

	//Rotation
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset")
	void GET_Rotation(const FString& Address, const FRotator& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Packs")
	void GET_Rotation_Pack(const FString& Address, const TMap<FString, FRotator>& Map);

	//Color
	
	// UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset")
	// void OnOSCColorReceived(const FString& Address, const FLinearColor& Value);
};
