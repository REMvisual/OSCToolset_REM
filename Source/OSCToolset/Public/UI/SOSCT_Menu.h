// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff


#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"

/**
 * 
 */
DECLARE_DELEGATE(FOnReInitOSCT);

class OSCTOOLSET_API SOSCT_Menu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOSCT_Menu) {}
	SLATE_END_ARGS()
	
	FOnReInitOSCT OnReInitOSCT;

	void Construct(const FArguments& InArgs);

private:

	void SaveSettings();

	TSharedPtr<SEditableTextBox> ServerAddressTextBox;
	TSharedPtr<SSpinBox<int32>> ServerPortSpinBox;
	TSharedPtr<SCheckBox> MulticastLoopbackBox;

	TSharedPtr<SEditableTextBox> ClientAddressTextBox;
	TSharedPtr<SSpinBox<int32>> ClientPortSpinBox;
	
	TSharedPtr<SCheckBox> UseLocalIPV4Box;
	
	//FReply OnSaveSettings();
	FReply OnReinitializeOSCT();

	FString ServerAddress;
	int32 ServerPort;
	bool MulticastLoopback;

	FString ClientAddress;
	int32 ClientPort; 
	bool UseLocalIPV4;
};
