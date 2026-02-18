// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SOSCT_Menu.h"
#include "OSCT_Settings.h"
#include "OSCT_Master.h"

#include "SlateOptMacros.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h" // Required for GameViewport.

#include "GameFramework/PlayerController.h"
#include "UObject/UnrealType.h"

void SOSCT_Menu::Construct(const FArguments& InArgs)
{
	// Retrieve settings from config file
	const UOSCT_Settings* Config = GetDefault<UOSCT_Settings>();
	ServerAddress = Config->ServerAddress;
	ServerPort = Config->ServerPort;
    MulticastLoopback = Config->MulticastLoopback;

	ClientAddress = Config->ClientAddress;
	ClientPort = Config->ClientPort;
    
    UseLocalIPV4 = Config->UseLocalIPV4;
    
    if (GEngine && GEngine->GameViewport)
    {
        const int32 ui_padding = 5;
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        // Calculate one-third of the screen width
        const float WidgetWidth = ViewportSize.X / 3.0f;

        ChildSlot
            [
                SNew(SOverlay)
                    + SOverlay::Slot()
                    .HAlign(HAlign_Left)
                    [
                        SNew(SBox)
                            .WidthOverride(WidgetWidth)
                            .HeightOverride(400)
                            .Padding(150)
                            [
                                SNew(SVerticalBox)
                                    +SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("OSCToolset Settings"))
                                            .Font(FSlateFontInfo(FCoreStyle::GetDefaultFontStyle("Bold", 24)))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(FMargin(ui_padding, ui_padding, ui_padding, 50))
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("made by Eusebi Jucgla"))
                                            .Font(FSlateFontInfo(FCoreStyle::GetDefaultFontStyle("Regular", 9)))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("Server Address"))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SAssignNew(ServerAddressTextBox, SEditableTextBox)
                                            .Text(FText::FromString(ServerAddress))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("Server Port"))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SAssignNew(ServerPortSpinBox, SSpinBox<int32>)
                                            .Value(ServerPort)
                                            .MinValue(1)
                                            .MaxValue(65535)
                                            .OnValueChanged_Lambda([this](int32 NewValue) {ServerPort = NewValue; })
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("Multicast Loopback"))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SAssignNew(MulticastLoopbackBox, SCheckBox)
                                            .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState)
                                                {
                                                    MulticastLoopback = (NewState == ECheckBoxState::Checked);
                                                })
                                            .IsChecked_Lambda([this]() -> ECheckBoxState
                                                {
                                                    return MulticastLoopback ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                                                })
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(SBox)
                                            .HeightOverride(35) // Adjust thickness
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("Client Address"))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SAssignNew(ClientAddressTextBox, SEditableTextBox)
                                            .Text(FText::FromString(ClientAddress))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("Client Port"))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SAssignNew(ClientPortSpinBox, SSpinBox<int32>)
                                            .Value(ClientPort)
                                            .MinValue(1)
                                            .MaxValue(65535)
                                            .OnValueChanged_Lambda([this](int32 NewValue) {ClientPort = NewValue; })
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SNew(STextBlock)
                                            .Text(FText::FromString("Use Local IPV4"))
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(ui_padding)
                                    [
                                        SAssignNew(UseLocalIPV4Box, SCheckBox)
                                            .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState)
                                                {
                                                    UseLocalIPV4 = (NewState == ECheckBoxState::Checked);
                                                })
                                            .IsChecked_Lambda([this]() -> ECheckBoxState
                                                {
                                                    return UseLocalIPV4 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                                                })
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding(FMargin(ui_padding, 25, ui_padding, ui_padding))
                                    .HAlign(HAlign_Fill)
                                    [
                                        SNew(SButton)
                                            .Text(FText::FromString("Save and Re-Initialize OSCToolset"))
                                            .OnClicked(this, &SOSCT_Menu::OnReinitializeOSCT)
                                            .HAlign(HAlign_Fill)
                                    ]
                            ]
                    ]
            ];
    }
}

void SOSCT_Menu::SaveSettings()
{
    UOSCT_Settings* MutableConfig = GetMutableDefault<UOSCT_Settings>();

    MutableConfig->ServerAddress = ServerAddressTextBox->GetText().ToString();
    MutableConfig->ServerPort = ServerPortSpinBox->GetValue();
    MutableConfig->MulticastLoopback = (MulticastLoopbackBox->GetCheckedState() == ECheckBoxState::Checked);

    MutableConfig->ClientAddress = ClientAddressTextBox->GetText().ToString();
    MutableConfig->ClientPort = ClientPortSpinBox->GetValue();
    
    MutableConfig->UseLocalIPV4 = (UseLocalIPV4Box->GetCheckedState() == ECheckBoxState::Checked);
    
    MutableConfig->SaveConfig();
}

FReply SOSCT_Menu::OnReinitializeOSCT()
{
    SaveSettings();
    OnReInitOSCT.ExecuteIfBound();
    return FReply::Handled();
}