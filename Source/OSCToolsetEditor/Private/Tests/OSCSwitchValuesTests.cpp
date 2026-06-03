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

// Regression (the reported bug): table-derived case pins lost their wired connections on load/Play.
// Cause: pins were resolved LIVE from AddressTable during reconstruction, but the table asset isn't
// guaranteed available at that moment, so the pins (and their links) vanished. The serialized
// CachedEntries snapshot must keep the pins even when the live table is momentarily unavailable.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesCachedPinsSurviveTableUnavailable,
	"OSCToolset.Editor.SwitchValues.TablePinsSurviveReconstructWithoutTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesCachedPinsSurviveTableUnavailable::RunTest(const FString&)
{
	using namespace OSCSwitchValuesTestHelpers;

	UDataTable* DT = NewObject<UDataTable>(GetTransientPackage());
	DT->RowStruct = FOSCT_ReceiverRow::StaticStruct();
	{ FOSCT_ReceiverRow R; R.Address = TEXT("KnobA1"); R.ModuleType = EOSCT_ModuleType::FLOAT; DT->AddRow(FName(TEXT("KnobA1")), R); }
	{ FOSCT_ReceiverRow R; R.Address = TEXT("LFO1");   R.ModuleType = EOSCT_ModuleType::FLOAT; DT->AddRow(FName(TEXT("LFO1")), R); }

	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->AddressTable = DT;
	Node->AddressFilter = TEXT("KnobA1");
	// Simulates PostLoad capturing the snapshot while the table IS available.
	Node->RefreshCachedEntries();

	// Simulate a reconstruction where the AddressTable asset isn't available yet (load/compile-on-load).
	Node->AddressTable = nullptr;
	Node->AllocateDefaultPins();

	TestNotNull(TEXT("KnobA1 exec out survives table-unavailable reconstruct"),
		FindPin(Node, FName(TEXT("KnobA1")), EGPD_Output));
	TestNotNull(TEXT("KnobA1 value out survives table-unavailable reconstruct"),
		FindPin(Node, FName(TEXT("KnobA1_Value")), EGPD_Output));
	TestNull(TEXT("LFO1 still filtered out of the cache"),
		FindPin(Node, FName(TEXT("LFO1")), EGPD_Output));
	return true;
}

// RefreshCachedEntries snapshots inline + filtered-table addresses (deduped).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesRefreshCache,
	"OSCToolset.Editor.SwitchValues.RefreshCachedEntriesSnapshotsAddresses",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesRefreshCache::RunTest(const FString&)
{
	UDataTable* DT = NewObject<UDataTable>(GetTransientPackage());
	DT->RowStruct = FOSCT_ReceiverRow::StaticStruct();
	{ FOSCT_ReceiverRow R; R.Address = TEXT("KnobA1"); R.ModuleType = EOSCT_ModuleType::FLOAT; DT->AddRow(FName(TEXT("KnobA1")), R); }
	{ FOSCT_ReceiverRow R; R.Address = TEXT("LFO1");   R.ModuleType = EOSCT_ModuleType::FLOAT; DT->AddRow(FName(TEXT("LFO1")), R); }

	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->AddressTable = DT;
	Node->AddressFilter = TEXT("Knob*");
	Node->Addresses = { SV_Addr(TEXT("Tint"), EOSCT_ModuleType::COLOR) };
	Node->RefreshCachedEntries();

	// KnobA1 (table, passes filter) + Tint (inline) = 2; LFO1 filtered out.
	TestEqual(TEXT("cache holds 2 entries"), Node->CachedEntries.Num(), 2);
	bool bHasKnob = false, bHasTint = false, bHasLFO = false;
	for (const FOSCT_NodeAddress& E : Node->CachedEntries)
	{
		if (E.Address == TEXT("KnobA1")) bHasKnob = true;
		if (E.Address == TEXT("Tint"))   bHasTint = true;
		if (E.Address == TEXT("LFO1"))   bHasLFO  = true;
	}
	TestTrue(TEXT("KnobA1 cached"), bHasKnob);
	TestTrue(TEXT("Tint cached"), bHasTint);
	TestFalse(TEXT("LFO1 not cached"), bHasLFO);
	return true;
}

// Migration / load-race safety net: the cache can be rebuilt from the node's own persisted pins, recovering
// each address AND its type — so case pins survive even when the AddressTable asset hasn't loaded yet.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesDeriveFromPins,
	"OSCToolset.Editor.SwitchValues.DeriveCacheFromPins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesDeriveFromPins::RunTest(const FString&)
{
	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->Addresses = { SV_Addr(TEXT("KnobA1"), EOSCT_ModuleType::FLOAT),
	                    SV_Addr(TEXT("Tint"),   EOSCT_ModuleType::COLOR),
	                    SV_Addr(TEXT("Trig"),   EOSCT_ModuleType::EVENT) };
	Node->RefreshCachedEntries();   // inline-only -> cache = 3
	Node->AllocateDefaultPins();    // build pins from cache

	Node->CachedEntries.Reset();    // simulate an empty cache on load (pre-fix / wiped asset)
	Node->DeriveCachedEntriesFromPins();

	TestEqual(TEXT("derived 3 entries from pins"), Node->CachedEntries.Num(), 3);
	auto TypeOf = [&](const TCHAR* A, EOSCT_ModuleType& Out) -> bool
	{
		for (const FOSCT_NodeAddress& E : Node->CachedEntries) { if (E.Address == A) { Out = E.Type; return true; } }
		return false;
	};
	EOSCT_ModuleType T;
	TestTrue(TEXT("KnobA1 recovered"), TypeOf(TEXT("KnobA1"), T)); if (TypeOf(TEXT("KnobA1"), T)) TestEqual(TEXT("KnobA1 is FLOAT"), (int32)T, (int32)EOSCT_ModuleType::FLOAT);
	TestTrue(TEXT("Tint recovered"),   TypeOf(TEXT("Tint"),   T)); if (TypeOf(TEXT("Tint"),   T)) TestEqual(TEXT("Tint is COLOR"),  (int32)T, (int32)EOSCT_ModuleType::COLOR);
	TestTrue(TEXT("Trig recovered"),   TypeOf(TEXT("Trig"),   T)); if (TypeOf(TEXT("Trig"),   T)) TestEqual(TEXT("Trig is EVENT"),  (int32)T, (int32)EOSCT_ModuleType::EVENT);
	return true;
}

// Guard: a not-yet-loaded table (RowStruct still null) must NOT wipe a good snapshot — this is the exact
// load-order race that dropped the case pins. A genuinely sourceless node, however, does clear.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCSwitchValuesRefreshGuard,
	"OSCToolset.Editor.SwitchValues.RefreshDoesNotWipeOnUnloadedTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCSwitchValuesRefreshGuard::RunTest(const FString&)
{
	// Table assigned but unreadable (no RowStruct) == mid-load. Cache must be preserved.
	UDataTable* Unloaded = NewObject<UDataTable>(GetTransientPackage()); // RowStruct deliberately left null
	UK2Node_OSCSwitchValues* Node = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Node->AddressTable = Unloaded;
	Node->CachedEntries = { SV_Addr(TEXT("KnobA1"), EOSCT_ModuleType::FLOAT) }; // pretend deserialized snapshot
	Node->RefreshCachedEntries();
	TestEqual(TEXT("cache preserved when table not loaded"), Node->CachedEntries.Num(), 1);

	// No source at all -> cache legitimately clears.
	UK2Node_OSCSwitchValues* Empty = NewObject<UK2Node_OSCSwitchValues>(GetTransientPackage());
	Empty->CachedEntries = { SV_Addr(TEXT("Stale"), EOSCT_ModuleType::FLOAT) };
	Empty->RefreshCachedEntries();
	TestEqual(TEXT("cache cleared when no source configured"), Empty->CachedEntries.Num(), 0);
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
