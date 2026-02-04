// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OSCT_ETypes.h"
#include "OSCT_Router.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UOSCT_Router : public UInterface
{
	GENERATED_BODY()
};

class OSCTOOLSET_API IOSCT_Router
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Message received main event
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Base")
	void OnOSCMessageReceived(const FString& Address, const FOSCMessage& Message);
	
	//OSCTModules lifecycle
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Base")
	void OnRegisterOSCTModule(const FOSCT_Module& Module);
	
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Base")
	void OnOSCTModuleInit(const FOSCT_Module& Module);
	
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Base")
	void OnRemovedOSCTModule(const FOSCT_Module& Module);
	
	//Receivers
	
	// Event
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Event(const FOSCT_Module& Module);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_Event_Pack(const FOSCT_Module& Module, const TMap<FString, bool>& Map);
	
	// Float
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Float(const FOSCT_Module& Module, const float Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Tick")
	void GET_Float_Tick(const FOSCT_Module& Module, const float Value);
	
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Receivers|Packs")
	void GET_Float_Pack(const FOSCT_Module& Module, const TMap<FString, float>& Map);
	UFUNCTION(BlueprintNativeEvent, Category = "OSCToolset|Receivers|Packs|Tick")
	void GET_Float_Pack_Tick(const FOSCT_Module& Module, const TMap<FString, float>& Map);

	//Vector 2
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Vector2(const FOSCT_Module& Module, const FVector2D& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Tick")
	void GET_Vector2_Tick(const FOSCT_Module& Module, const FVector2D& Value);
	
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_Vector2_Pack(const FOSCT_Module& Module, const TMap<FString, FVector2D>& Map);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs|Tick")
	void GET_Vector2_Pack_Tick(const FOSCT_Module& Module, const TMap<FString, FVector2D>& Map);

	//Vector 3
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Vector3(const FOSCT_Module& Module, const FVector& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Tick")
	void GET_Vector3_Tick(const FOSCT_Module& Module, const FVector& Value);
	
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_Vector3_Pack(const FOSCT_Module& Module, const TMap<FString, FVector>& Map);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs|Tick")
	void GET_Vector3_Pack_Tick(const FOSCT_Module& Module, const TMap<FString, FVector>& Map);

	//Rotation
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Rotation(const FOSCT_Module& Module, const FRotator& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Tick")
	void GET_Rotation_Tick(const FOSCT_Module& Module, const FRotator& Value);
	
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_Rotation_Pack(const FOSCT_Module& Module, const TMap<FString, FRotator>& Map);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs|Tick")
	void GET_Rotation_Pack_Tick(const FOSCT_Module& Module, const TMap<FString, FRotator>& Map);

	//Color
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Color(const FOSCT_Module& Module, const FLinearColor& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Tick")
	void GET_Color_Tick(const FOSCT_Module& Module, const FLinearColor& Value);

	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_Color_Pack(const FOSCT_Module& Module, const TMap<FString, FLinearColor>& Map);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs|Tick")
	void GET_Color_Pack_Tick(const FOSCT_Module& Module, const TMap<FString, FLinearColor>& Map);

	//Transform
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Transform(const FOSCT_Module& Module, const FTransform& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Tick")
	void GET_Transform_Tick(const FOSCT_Module& Module, const FTransform& Value);

	
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_Transform_Pack(const FOSCT_Module& Module, const TMap<FString, FTransform>& Map);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs|Tick")
	void GET_Transform_Pack_Tick(const FOSCT_Module& Module, const TMap<FString, FTransform>& Map);

	//String
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_String(const FOSCT_Module& Module, const FString& Value);
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers|Packs")
	void GET_String_Pack(const FOSCT_Module& Module, const TMap<FString, FString>& Map);
	
	//Notes
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Receivers")
	void GET_Notes(const FOSCT_Module& Module, const FOSCT_Note& Value);
	
	
	//Senders
	//In case you want to combine values and connecting actors
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Senders")
	void SEND_Event(const FOSCT_Module& Module);
	
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Senders")
	void SEND_Float(const FOSCT_Module& Module, const float Value);
	
	UFUNCTION(BlueprintNativeEvent, Category="OSCToolset|Senders")
	void SEND_String(const FOSCT_Module& Module, const FString& Value);
};
