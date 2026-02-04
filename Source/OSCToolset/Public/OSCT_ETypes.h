// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetStringLibrary.h"

#include "OSCT_ETypes.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EOSCT_Role : uint8
{
	RECEIVER	UMETA(DisplayName = "Receiver"),
	SENDER		UMETA(DisplayName = "Sender"),
};

UENUM(BlueprintType)
enum class EOSCT_ModuleType : uint8
{
	EVENT	UMETA(DisplayName = "Event"),
	FLOAT	UMETA(DisplayName = "Float"),
	VEC2	UMETA(DisplayName = "Vector 2"),
	VEC3	UMETA(DisplayName = "Vector 3"),
	COLOR	UMETA(DisplayName = "Color"),
	TRANSFORM	UMETA(DisplayName = "Transform"),	
	STRING		UMETA(DisplayName = "String"),
	ROTATION	UMETA(DisplayName = "Rotation"),
	NOTE		UMETA(DisplayName = "Note"),
};

UENUM(BlueprintType)
enum class EOSCT_Sender_Type : uint8
{
	EVENT	UMETA(DisplayName = "Event"),
	FLOAT	UMETA(DisplayName = "Float"),
	STRING	UMETA(DisplayName = "String"),
};

USTRUCT(BlueprintType)
struct FOSCT_Note
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	int32 Pitch = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	int32 Velocity = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	int32 Voices = 1;
};


USTRUCT(BlueprintType)
struct FOSCT_ModuleDebug
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	bool Debug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	bool PrintOnScreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	bool PrintOnLog = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	bool DebugAddress = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	FColor DebugColor = FColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug", meta = (EditCondition = "Debug"))
	float DebugDuration = 2.0f;
};


USTRUCT(BlueprintType)
struct FOSCT_Tick
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick")
	bool bEnable = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", 
		meta = (
			EditCondition = "bEnable", 
			EditConditionHides))
	float InterpolationSpeed = 5.0f;
};

USTRUCT(BlueprintType)
struct FOSCT_Module :  public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FString FormattedAddress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	FString Address;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	EOSCT_Role Role = EOSCT_Role::RECEIVER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	EOSCT_ModuleType ModuleType = EOSCT_ModuleType::EVENT;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset", 
		meta = (
			EditCondition = "ModuleType != EOSCT_ModuleType::STRING && ModuleType != EOSCT_ModuleType::NOTE"))
	bool Pack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick", 
		meta=(
			EditCondition = "ModuleType != EOSCT_ModuleType::EVENT && ModuleType != EOSCT_ModuleType::STRING && ModuleType != EOSCT_ModuleType::NOTE",
			EditConditionHides
			))
	FOSCT_Tick Tick;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	FOSCT_ModuleDebug Debug;
	
	
	// Initialize string types.
	FOSCT_Module()
		: FormattedAddress(TEXT(""))
		, Address(TEXT(""))
	{}
};


USTRUCT(BlueprintType)
struct FOSCT_ModuleLink
{
	GENERATED_BODY()
	
	TWeakObjectPtr<UObject> Owner;
	
	UPROPERTY()
	FOSCT_Module Data;
	
	UObject* GetOwner() const { return Owner.Get(); }
	bool HasValidOwner() const { return Owner.IsValid(); }
	// This allows AddUnique and Contains to work
	bool operator==(const FOSCT_ModuleLink& Other) const
	{
		return GetOwner() == Other.GetOwner(); 
	}
};



USTRUCT(BlueprintType)
struct FOSCT_ReceiverLink: public FOSCT_ModuleLink
{
	GENERATED_BODY()

	//Data containers to lerp from Master instead of per component / osct module. 
	FVector CurrentValue = FVector::ZeroVector; // Used for Float, Vec2, Vec3, Transforms
	FVector TargetValue  = FVector::ZeroVector;

	//For scale
	FVector CurrentScale = FVector::OneVector; // Scale should default to 1!
	FVector TargetScale  = FVector::OneVector;
    
	//For color
	FLinearColor CurrentColor = FLinearColor::White;
	FLinearColor TargetColor = FLinearColor::White;
	
	FRotator CurrentRotation = FRotator::ZeroRotator;  // Used ONLY for Rotation modules
	FRotator TargetRotation = FRotator::ZeroRotator;
	
	bool bNeedsInterpolation = false;
	bool bInitialized = false;
};


USTRUCT(BlueprintType)
struct FOSCT_SenderLink: public FOSCT_ModuleLink
{
	GENERATED_BODY()
	//TODO-Maybe Implement a send rate?
};



class OSCTOOLSET_API OSCT_ETypes
{
public:
	OSCT_ETypes();
	~OSCT_ETypes();
};
