#if WITH_DEV_AUTOMATION_TESTS

#include "K2Node_OSCSwitchValues.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "OSCT_Modules.h" // FOSCT_ReceiverRow
#include "Interfaces/OSCT_Router.h" // GET_All / GET_All_Tick

static FOSCT_NodeAddress SV_Addr(const TCHAR* A, EOSCT_ModuleType T)
{
	FOSCT_NodeAddress E; E.Address = A; E.Type = T; return E;
}

namespace OSCSwitchValuesTestHelpers
{
	static UEdGraphPin* FindPin(const UK2Node_OSCSwitchValues* Node, FName Name, EEdGraphPinDirection Dir)
	{
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin->Direction == Dir && Pin->PinName == Name) return Pin;
		}
		return nullptr;
	}
	static int32 CountExecOutputs(const UK2Node_OSCSwitchValues* Node)
	{
		int32 N = 0;
		for (const UEdGraphPin* Pin : Node->Pins)
			if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec) ++N;
		return N;
	}
}

// Each numeric address yields an exec output + a value pin; plus Default exec, Receiver in, exec in.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesFloatPins,
	"OSCToolset.Editor.SwitchValues.AddressesYieldExecAndValuePins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesFloatPins::RunTest(const FString&)
{
	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->Addresses = { SV_Addr(TEXT("KnobA1"), EOSCT_ModuleType::FLOAT), SV_Addr(TEXT("Main"), EOSCT_ModuleType::FLOAT) };
	Node->AllocateDefaultPins();

	using namespace OSCSwitchValuesTestHelpers;
	TestNotNull(TEXT("exec in"), FindPin(Node, UEdGraphSchema_K2::PN_Execute, EGPD_Input));
	TestNotNull(TEXT("Receiver in"), FindPin(Node, FName(TEXT("Receiver")), EGPD_Input));
	TestNotNull(TEXT("Default exec out"), FindPin(Node, FName(TEXT("Default")), EGPD_Output));

	TestNotNull(TEXT("KnobA1 exec out"), FindPin(Node, FName(TEXT("KnobA1")), EGPD_Output));
	TestNotNull(TEXT("Main exec out"), FindPin(Node, FName(TEXT("Main")), EGPD_Output));

	UEdGraphPin* KnobVal = FindPin(Node, FName(TEXT("KnobA1_Value")), EGPD_Output);
	TestNotNull(TEXT("KnobA1 value out"), KnobVal);
	if (KnobVal) TestEqual(TEXT("KnobA1 value is float"), KnobVal->PinType.PinCategory, UEdGraphSchema_K2::PC_Real);
	TestNotNull(TEXT("Main value out"), FindPin(Node, FName(TEXT("Main_Value")), EGPD_Output));

	// 3 exec outputs: Default + KnobA1 + Main
	TestEqual(TEXT("3 exec outputs"), CountExecOutputs(Node), 3);
	return true;
}

// EVENT-type addresses get an exec output but NO value pin.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesEventExecOnly,
	"OSCToolset.Editor.SwitchValues.EventTypeIsExecOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesEventExecOnly::RunTest(const FString&)
{
	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->Addresses = { SV_Addr(TEXT("Trigger"), EOSCT_ModuleType::EVENT) };
	Node->AllocateDefaultPins();

	using namespace OSCSwitchValuesTestHelpers;
	TestNotNull(TEXT("Trigger exec out"), FindPin(Node, FName(TEXT("Trigger")), EGPD_Output));
	TestNull(TEXT("no Trigger value pin"), FindPin(Node, FName(TEXT("Trigger_Value")), EGPD_Output));
	return true;
}

// Bool type produces a boolean value pin.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesBoolPin,
	"OSCToolset.Editor.SwitchValues.BoolTypeBecomesBoolValuePin",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesBoolPin::RunTest(const FString&)
{
	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->Addresses = { SV_Addr(TEXT("Mute"), EOSCT_ModuleType::BOOL) };
	Node->AllocateDefaultPins();

	using namespace OSCSwitchValuesTestHelpers;
	UEdGraphPin* Val = FindPin(Node, FName(TEXT("Mute_Value")), EGPD_Output);
	TestNotNull(TEXT("Mute value pin"), Val);
	if (Val) TestEqual(TEXT("Mute value is bool"), Val->PinType.PinCategory, UEdGraphSchema_K2::PC_Boolean);
	return true;
}

// Filter restricts table rows here too (shared semantics).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesFilter,
	"OSCToolset.Editor.SwitchValues.AddressFilterRestrictsTableRows",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesFilter::RunTest(const FString&)
{
	UDataTable* DT = NewObject<UDataTable>(GetTransientPackage());
	DT->RowStruct = FOSCT_ReceiverRow::StaticStruct();
	{ FOSCT_ReceiverRow R; R.Address = TEXT("KnobA1"); R.ModuleType = EOSCT_ModuleType::FLOAT; DT->AddRow(FName(TEXT("KnobA1")), R); }
	{ FOSCT_ReceiverRow R; R.Address = TEXT("LFO1");   R.ModuleType = EOSCT_ModuleType::FLOAT; DT->AddRow(FName(TEXT("LFO1")), R); }

	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->AddressTable = DT;
	Node->AddressFilter = TEXT("KnobA1");
	Node->AllocateDefaultPins();

	using namespace OSCSwitchValuesTestHelpers;
	TestNotNull(TEXT("KnobA1 exec out"), FindPin(Node, FName(TEXT("KnobA1")), EGPD_Output));
	TestNull(TEXT("LFO1 filtered out"), FindPin(Node, FName(TEXT("LFO1")), EGPD_Output));
	// Default + KnobA1 only
	TestEqual(TEXT("2 exec outputs after filter"), CountExecOutputs(Node), 2);
	return true;
}

// bIncludeDefault=false removes the Default exec pin.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesNoDefault,
	"OSCToolset.Editor.SwitchValues.IncludeDefaultTogglesDefaultPin",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesNoDefault::RunTest(const FString&)
{
	using namespace OSCSwitchValuesTestHelpers;

	UK2Node_OSCSwitchValues* WithDef = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	WithDef->Addresses = { SV_Addr(TEXT("KnobA1"), EOSCT_ModuleType::FLOAT) };
	WithDef->bIncludeDefault = true;
	WithDef->AllocateDefaultPins();
	TestNotNull(TEXT("Default present when on"), FindPin(WithDef, FName(TEXT("Default")), EGPD_Output));

	UK2Node_OSCSwitchValues* NoDef = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	NoDef->Addresses = { SV_Addr(TEXT("KnobA1"), EOSCT_ModuleType::FLOAT) };
	NoDef->bIncludeDefault = false;
	NoDef->AllocateDefaultPins();
	TestNull(TEXT("Default absent when off"), FindPin(NoDef, FName(TEXT("Default")), EGPD_Output));
	return true;
}

// Inline list is multi-value: a float and a color address in one node produce correctly-typed value pins.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesMultiValueInline,
	"OSCToolset.Editor.SwitchValues.InlineListIsMultiValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesMultiValueInline::RunTest(const FString&)
{
	using namespace OSCSwitchValuesTestHelpers;

	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->Addresses = { SV_Addr(TEXT("KnobA1"), EOSCT_ModuleType::FLOAT), SV_Addr(TEXT("Tint"), EOSCT_ModuleType::COLOR) };
	Node->AllocateDefaultPins();

	UEdGraphPin* KnobVal = FindPin(Node, FName(TEXT("KnobA1_Value")), EGPD_Output);
	UEdGraphPin* TintVal = FindPin(Node, FName(TEXT("Tint_Value")), EGPD_Output);
	TestNotNull(TEXT("KnobA1 value pin"), KnobVal);
	TestNotNull(TEXT("Tint value pin"), TintVal);
	if (KnobVal) TestEqual(TEXT("KnobA1 float"), KnobVal->PinType.PinCategory, UEdGraphSchema_K2::PC_Real);
	if (TintVal) TestEqual(TEXT("Tint struct"), TintVal->PinType.PinCategory, UEdGraphSchema_K2::PC_Struct);
	return true;
}

// The universal GET_All / GET_All_Tick events exist on the OSCT Router interface.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCRouterGetAllExists,
	"OSCToolset.Editor.Router.GetAllEventsExist",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCRouterGetAllExists::RunTest(const FString&)
{
	UClass* Iface = UOSCT_Router::StaticClass();
	TestNotNull(TEXT("GET_All exists on IOSCT_Router"), Iface->FindFunctionByName(FName(TEXT("GET_All"))));
	TestNotNull(TEXT("GET_All_Tick exists on IOSCT_Router"), Iface->FindFunctionByName(FName(TEXT("GET_All_Tick"))));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
