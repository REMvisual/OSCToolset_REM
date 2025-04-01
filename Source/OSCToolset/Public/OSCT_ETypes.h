// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#pragma once

#include "CoreMinimal.h"
#include "OSCT_ETypes.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EOSCT_Role_Type : uint8
{
	RECEIVER	UMETA(DisplayName = "Receiver"),
	SENDER		UMETA(DisplayName = "Sender"),
};

UENUM(BlueprintType)
enum class EOSCT_Module_Type : uint8
{
	EVENT	UMETA(DisplayName = "Event"),
	FLOAT	UMETA(DisplayName = "Float"),
	VEC2	UMETA(DisplayName = "Vector 2"),
	VEC3	UMETA(DisplayName = "Vector 3"),
	COLOR	UMETA(DisplayName = "Color"),
	TRANSFORM	UMETA(DisplayName = "Transform"),	
	STRING		UMETA(DisplayName = "String"),
	ROTATION	UMETA(DisplayName = "Rotation"),
	MIDI		UMETA(DisplayName = "MIDI"),

};

UENUM(BlueprintType)
enum class EOSCT_Sender_Type : uint8
{
	EVENT	UMETA(DisplayName = "Event"),
	FLOAT	UMETA(DisplayName = "Float"),
	STRING	UMETA(DisplayName = "String"),
};

class OSCTOOLSET_API OSCT_ETypes
{
public:
	OSCT_ETypes();
	~OSCT_ETypes();
};
