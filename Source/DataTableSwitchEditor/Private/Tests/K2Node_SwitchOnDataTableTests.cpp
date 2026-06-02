#if WITH_DEV_AUTOMATION_TESTS

#include "K2Node_SwitchOnDataTable.h"
#include "Engine/DataTable.h"
#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"

namespace DataTableSwitchTests
{
	static UDataTable* MakeTestDataTableWithRowNames(const TArray<FName>& RowNames)
	{
		UDataTable* DT = NewObject<UDataTable>(GetTransientPackage(), NAME_None, RF_Transient);
		DT->RowStruct = FTableRowBase::StaticStruct();
		for (const FName& RowName : RowNames)
		{
			FTableRowBase Row;
			DT->AddRow(RowName, Row);
		}
		return DT;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchEmptyTableYieldsNoPins,
	"DataTableSwitch.Editor.EmptyTableYieldsNoPins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchEmptyTableYieldsNoPins::RunTest(const FString&)
{
	UDataTable* DT = DataTableSwitchTests::MakeTestDataTableWithRowNames({});
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(GetTransientPackage());
	Node->DataTable = DT;
	Node->RefreshCaseValues();
	TestEqual(TEXT("PinNames is empty for empty DataTable"), Node->PinNames.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchRowNamesBecomePins,
	"DataTableSwitch.Editor.RowNamesBecomePinsWhenColumnEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchRowNamesBecomePins::RunTest(const FString&)
{
	const TArray<FName> Rows = { FName("RowA"), FName("RowB"), FName("RowC") };
	UDataTable* DT = DataTableSwitchTests::MakeTestDataTableWithRowNames(Rows);
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(GetTransientPackage());
	Node->DataTable = DT;
	Node->ColumnName = NAME_None;
	Node->RefreshCaseValues();
	TestEqual(TEXT("3 pins"), Node->PinNames.Num(), 3);
	for (const FName& Expected : Rows)
	{
		TestTrue(*FString::Printf(TEXT("contains pin %s"), *Expected.ToString()), Node->PinNames.Contains(Expected));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchNullTableYieldsNoPins,
	"DataTableSwitch.Editor.NullDataTableYieldsNoPins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchNullTableYieldsNoPins::RunTest(const FString&)
{
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(GetTransientPackage());
	Node->DataTable = nullptr;
	Node->RefreshCaseValues();
	TestEqual(TEXT("no pins when null"), Node->PinNames.Num(), 0);
	return true;
}

#include "OSCFixtureRow.h"

static UDataTable* MakeOSCFixtureTable()
{
	UDataTable* DT = NewObject<UDataTable>(GetTransientPackage(), NAME_None, RF_Transient);
	DT->RowStruct = FOSCFixtureRow::StaticStruct();

	FOSCFixtureRow R1; R1.Address = TEXT("LFO1"); R1.ModuleType = TEXT("FLOAT");
	FOSCFixtureRow R2; R2.Address = TEXT("LFO2"); R2.ModuleType = TEXT("FLOAT");
	FOSCFixtureRow R3; R3.Address = TEXT("Audio1"); R3.ModuleType = TEXT("FLOAT");
	DT->AddRow(FName(TEXT("/project1/OSCT_SEND_LFO_0")), R1);
	DT->AddRow(FName(TEXT("/project1/OSCT_SEND_LFO_1")), R2);
	DT->AddRow(FName(TEXT("/project1/OSCT_SEND_Audio_0")), R3);
	return DT;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchAddressColumnBecomesPins,
	"DataTableSwitch.Editor.AddressColumnBecomesPins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchAddressColumnBecomesPins::RunTest(const FString&)
{
	UDataTable* DT = MakeOSCFixtureTable();
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(GetTransientPackage());
	Node->DataTable = DT;
	Node->ColumnName = FName(TEXT("Address"));
	Node->RefreshCaseValues();

	TestEqual(TEXT("3 pins"), Node->PinNames.Num(), 3);
	TestTrue(TEXT("LFO1 pin"), Node->PinNames.Contains(FName(TEXT("LFO1"))));
	TestTrue(TEXT("LFO2 pin"), Node->PinNames.Contains(FName(TEXT("LFO2"))));
	TestTrue(TEXT("Audio1 pin"), Node->PinNames.Contains(FName(TEXT("Audio1"))));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchCaseInsensitiveColumnFallback,
	"DataTableSwitch.Editor.CaseInsensitiveColumnFallback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchCaseInsensitiveColumnFallback::RunTest(const FString&)
{
	UDataTable* DT = MakeOSCFixtureTable();
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(GetTransientPackage());
	Node->DataTable = DT;
	Node->ColumnName = FName(TEXT("address"));
	Node->RefreshCaseValues();

	TestEqual(TEXT("3 pins via case-insensitive fallback"), Node->PinNames.Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchMissingColumnYieldsNoPins,
	"DataTableSwitch.Editor.MissingColumnYieldsNoPins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchMissingColumnYieldsNoPins::RunTest(const FString&)
{
	UDataTable* DT = MakeOSCFixtureTable();
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(GetTransientPackage());
	Node->DataTable = DT;
	Node->ColumnName = FName(TEXT("ColumnThatDoesNotExist"));
	Node->RefreshCaseValues();

	TestEqual(TEXT("no pins for missing column"), Node->PinNames.Num(), 0);
	return true;
}

#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "GameFramework/Actor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchAllocatePins,
	"DataTableSwitch.Editor.AllocateDefaultPinsCreatesExecOutputs",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchAllocatePins::RunTest(const FString&)
{
	// A K2Node's AllocateDefaultPins can hit FBlueprintEditorUtils::FindBlueprintForNodeChecked,
	// so the node MUST live in an EdGraph that is owned by a real Blueprint. A transient
	// package outer is not sufficient.
	UBlueprint* BP = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),
		GetTransientPackage(),
		FName(TEXT("BP_DataTableSwitchTest")),
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass());
	if (!BP || BP->UbergraphPages.Num() == 0)
	{
		AddError(TEXT("Failed to create transient Blueprint or it has no Ubergraph"));
		return false;
	}
	UEdGraph* Graph = BP->UbergraphPages[0];

	UDataTable* DT = MakeOSCFixtureTable();
	UK2Node_SwitchOnDataTable* Node = NewObject<UK2Node_SwitchOnDataTable>(Graph);
	Graph->AddNode(Node, /*bUserAction*/ false, /*bSelectNewNode*/ false);
	Node->DataTable = DT;
	Node->ColumnName = FName(TEXT("Address"));
	Node->RefreshCaseValues();
	Node->AllocateDefaultPins();

	int32 ExecOutputs = 0;
	int32 StringInputs = 0;
	int32 ExecInputs = 0;
	for (const UEdGraphPin* Pin : Node->Pins)
	{
		if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
		{
			++ExecOutputs;
		}
		if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_String)
		{
			++StringInputs;
		}
		if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
		{
			++ExecInputs;
		}
	}

	TestEqual(TEXT("1 exec input"), ExecInputs, 1);
	TestEqual(TEXT("1 string Selection input"), StringInputs, 1);
	TestEqual(TEXT("4 exec outputs (default + 3 cases)"), ExecOutputs, 4);

	TestNotNull(TEXT("LFO1 exec pin"), Node->FindPin(FName(TEXT("LFO1"))));
	TestNotNull(TEXT("Audio1 exec pin"), Node->FindPin(FName(TEXT("Audio1"))));
	return true;
}

#include "Kismet/KismetStringLibrary.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDataTableSwitchFunctionResolvable,
	"DataTableSwitch.Editor.SwitchComparatorFunctionExists",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDataTableSwitchFunctionResolvable::RunTest(const FString&)
{
	const FName CIName = TEXT("NotEqual_StriStri");
	const FName CSName = TEXT("NotEqual_StrStr");

	UFunction* CIFunc = UKismetStringLibrary::StaticClass()->FindFunctionByName(CIName);
	UFunction* CSFunc = UKismetStringLibrary::StaticClass()->FindFunctionByName(CSName);

	TestNotNull(TEXT("case-insensitive comparator exists"), CIFunc);
	TestNotNull(TEXT("case-sensitive comparator exists"), CSFunc);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
