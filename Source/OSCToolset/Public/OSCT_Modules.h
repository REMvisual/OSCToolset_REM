// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSCT_ETypes.h"
#include "OSCAddress.h"
#include "OSCT_Modules.generated.h"

USTRUCT(BlueprintType)
struct FOSCT_ModuleDebug
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	bool PrintOnScreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	bool PrintOnLog = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	FColor DebugColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	float DebugDuration = 2.0f;
};

USTRUCT(BlueprintType)
struct FOSCT_ReceiverTick
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick")
	bool bEnable = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick")
	float InterpolationSpeed = 5.0f;
};


USTRUCT(BlueprintType)
struct FOSCT_Receiver
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString FormattedAddress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	FString Address;
	
	UPROPERTY()
	EOSCT_Role Role = EOSCT_Role::RECEIVER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	EOSCT_ModuleType ModuleType = EOSCT_ModuleType::FLOAT;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	bool Pack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Tick")
	FOSCT_ReceiverTick Tick;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	FOSCT_ModuleDebug Debug;
};


USTRUCT(BlueprintType)
struct FOSCT_ReceiverLink
{
	GENERATED_BODY()
	TWeakObjectPtr<UObject> Owner;
	
	UPROPERTY()
	FOSCT_Receiver Data;

	bool bNeedsInterpolation = false;
	bool bInitialized = false;
	
	UObject* GetOwner() const { return Owner.Get(); }
	bool HasValidOwner() const { return Owner.IsValid(); }
	// This allows AddUnique and Contains to work
	bool operator==(const FOSCT_ReceiverLink& Other) const
	{
		return GetOwner() == Other.GetOwner() && Data.Address == Other.Data.Address;
	}

};

USTRUCT()
struct FOSCT_IndexList
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int32> Indices;
};



////////////// Receiver Types
////////////// Receiver Types
////////////// Receiver Types

/// EVENT
/// EVENT
/// EVENT
USTRUCT()
struct FOSCT_EventLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()

	UPROPERTY()
	bool CurrentValue = false;
	UPROPERTY()
	bool TargetValue = false;
	
	void Interpolate(float DeltaTime){};
};
USTRUCT()
struct FOSCT_EventPackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()

	UPROPERTY()
	TMap < FString, bool > CurrentValue;
	UPROPERTY()
	TMap < FString, bool > TargetValue;
	
	void Interpolate(float DeltaTime){};
};

/// FLOAT
/// FLOAT
/// FLOAT
USTRUCT()
struct FOSCT_FloatLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = float;
	
	UPROPERTY()
	float CurrentValue = 0.0f;
	UPROPERTY()
	float TargetValue = 0.0f;

	void Interpolate(float DeltaTime)
	{
		CurrentValue = FMath::FInterpTo(CurrentValue, TargetValue, DeltaTime, Data.Tick.InterpolationSpeed);
	};
	
	bool IsSettled() const {
		return FMath::IsNearlyEqual(CurrentValue, TargetValue, 0.001f);
	}
	
};
USTRUCT()
struct FOSCT_FloatPackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = TMap < FString, float >;
	
	UPROPERTY()
	TMap < FString, float > CurrentValue;
	UPROPERTY()
	TMap < FString, float > TargetValue;
	
	bool bIsCurrentlySettled = true;
	
	void Interpolate(float DeltaTime)
	{
		// Start by assuming we are settled
		bIsCurrentlySettled = true;
		
		for (auto& Pair : TargetValue)
		{
			float& Cur = CurrentValue.FindOrAdd(Pair.Key);
            
			// If we aren't at the target yet...
			if (!FMath::IsNearlyEqual(Cur, Pair.Value, 0.001f))
			{
				Cur = FMath::FInterpTo(Cur, Pair.Value, DeltaTime, Data.Tick.InterpolationSpeed);
				bIsCurrentlySettled = false;
			}
		}
	}
	bool IsSettled() const 
	{
		return bIsCurrentlySettled;
	}

};

/// VECTOR2
/// VECTOR2
/// VECTOR2
USTRUCT()
struct FOSCT_Vector2Link: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = FVector2D;
	
	UPROPERTY()
	FVector2D CurrentValue = FVector2D::ZeroVector;
	UPROPERTY()
	FVector2D TargetValue = FVector2D::ZeroVector;
	
	void Interpolate(float DeltaTime)
	{
		CurrentValue = FMath::Vector2DInterpTo(CurrentValue, TargetValue, DeltaTime, Data.Tick.InterpolationSpeed);
	}
	bool IsSettled() const {
		return CurrentValue.Equals(TargetValue, 0.01f);
	}

};
USTRUCT()
struct FOSCT_Vector2PackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = TMap < FString, FVector2D >;
	
	UPROPERTY()
	TMap < FString, FVector2D > CurrentValue;
	UPROPERTY()
	TMap < FString, FVector2D > TargetValue;
};

/// VECTOR3
/// VECTOR3
/// VECTOR3
USTRUCT()
struct FOSCT_Vector3Link: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = FVector;
	
	UPROPERTY()
	FVector CurrentValue = FVector::ZeroVector;
	UPROPERTY()
	FVector TargetValue = FVector::ZeroVector;
	
	void Interpolate(float DeltaTime)
	{
		CurrentValue = FMath::VInterpTo(CurrentValue, TargetValue, DeltaTime, Data.Tick.InterpolationSpeed);
	}
	bool IsSettled() const {
		return CurrentValue.Equals(TargetValue, 0.001f);
	}
};
USTRUCT()
struct FOSCT_Vector3PackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = TMap < FString, FVector >;
	
	UPROPERTY()
	TMap < FString, FVector > CurrentValue;
	UPROPERTY()
	TMap < FString, FVector > TargetValue;
};

/// ROTATION
/// ROTATION
/// ROTATION
USTRUCT()
struct FOSCT_RotationLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = FRotator;
	
	UPROPERTY()
	FRotator CurrentValue = FRotator::ZeroRotator;
	UPROPERTY()
	FRotator TargetValue = FRotator::ZeroRotator;

	void Interpolate(float DeltaTime)
	{
		CurrentValue = FMath::RInterpTo(CurrentValue, TargetValue, DeltaTime, Data.Tick.InterpolationSpeed);
	}
	bool IsSettled() const {
		return CurrentValue.Equals(TargetValue, 0.01f);
	}
};
USTRUCT()
struct FOSCT_RotationPackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = TMap < FString, FRotator >;
	
	UPROPERTY()
	TMap < FString, FRotator > CurrentValue;
	UPROPERTY()
	TMap < FString, FRotator > TargetValue;
};

/// COLOR
/// COLOR
/// COLOR
USTRUCT()
struct FOSCT_ColorLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = FLinearColor;
	
	UPROPERTY()
	FLinearColor CurrentValue = FLinearColor::White;
	UPROPERTY()
	FLinearColor TargetValue = FLinearColor::White;
};
USTRUCT()
struct FOSCT_ColorPackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = TMap < FString, FLinearColor > ;
	
	UPROPERTY()
	TMap < FString, FLinearColor > CurrentValue;
	UPROPERTY()
	TMap < FString, FLinearColor > TargetValue;
};

/// TRANSFORM
/// TRANSFORM
/// TRANSFORM
USTRUCT()
struct FOSCT_TransformLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = FTransform;
	
	UPROPERTY()
	FTransform CurrentValue = FTransform::Identity;
	UPROPERTY()
	FTransform TargetValue = FTransform::Identity;
	//
	// void Interpolate(float DeltaTime) {
	// 	float Speed = Data.Tick.InterpolationSpeed;
	// 	CurrentLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, Data.Tick.InterpolationSpeed);
	// 	CurrentRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, Data.Tick.InterpolationSpeed);
	// 	CurrentScale = FMath::VInterpTo(CurrentScale, TargetScale, DeltaTime, Data.Tick.InterpolationSpeed);
	// }
	//
	// bool IsSettled() const {
	// 	return CurrentLocation.Equals(TargetLocation, 0.001f) &&
	// 		   CurrentRotation.Equals(TargetRotation, 0.01f) &&
	// 		   CurrentScale.Equals(TargetScale, 0.001f);
	// }
};
USTRUCT()
struct FOSCT_TransformPackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()
	using ValueType = TMap < FString, FTransform >;
	
	UPROPERTY()
	TMap < FString, FTransform > CurrentValue;
	UPROPERTY()
	TMap < FString, FTransform > TargetValue;
};

/// STRING
/// STRING
/// STRING
USTRUCT()
struct FOSCT_StringLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()

	UPROPERTY()
	FString CurrentValue;
	UPROPERTY()
	FString TargetValue;

};
USTRUCT()
struct FOSCT_StringPackLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()

	UPROPERTY()
	TArray < FString > CurrentValue;
	UPROPERTY()
	TArray < FString > TargetValue;

};

/// NOTE
/// NOTE
/// NOTE
USTRUCT()
struct FOSCT_NoteLink: public FOSCT_ReceiverLink
{
	GENERATED_BODY()

	UPROPERTY()
	FOSCT_Note CurrentValue;
	UPROPERTY()
	FOSCT_Note TargetValue;
};

////////////// Sender
////////////// Sender
////////////// Sender
USTRUCT(BlueprintType)
struct FOSCT_Sender
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString FormattedAddress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset")
	FString Address;
	
	UPROPERTY()
	FOSCAddress CachedFullAddress;
	
	UPROPERTY()
	EOSCT_Role Role = EOSCT_Role::SENDER;

	UPROPERTY()
	EOSCT_SenderType Type = EOSCT_SenderType::EVENT;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCToolset|Debug")
	FOSCT_ModuleDebug Debug;
	
};


class OSCTOOLSET_API OSCT_Modules
{
public:
	OSCT_Modules();
	~OSCT_Modules();
};
