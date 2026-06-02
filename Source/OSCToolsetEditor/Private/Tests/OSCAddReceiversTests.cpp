#if WITH_DEV_AUTOMATION_TESTS

#include "K2Node_OSCAddReceivers.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/DataTable.h"
#include "Misc/AutomationTest.h"
#include "UObject/Package.h"

namespace OSCAddReceiversTestHelpers
{
	static UEdGraphPin* FindPin(const UK2Node_OSCAddReceivers* Node, FName Name, EEdGraphPinDirection Dir)
	{
		for (UEdGraphPin* Pin : Node->Pins)
			if (Pin->Direction == Dir && Pin->PinName == Name) return Pin;
		return nullptr;
	}
}

// The node exposes exec in/out + AddressTable (object) and AddressFilter (string) input pins, defaulted from Details.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOSCAddReceiversPins,
	"OSCToolset.Editor.AddReceivers.TableAndFilterAreInputPins",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOSCAddReceiversPins::RunTest(const FString&)
{
	using namespace OSCAddReceiversTestHelpers;

	UDataTable* DT = NewObject<UDataTable>(GetTransientPackage());
	UK2Node_OSCAddReceivers* Node = NewObject<UK2Node_OSCAddReceivers>(GetTransientPackage());
	Node->AddressTable = DT;
	Node->AddressFilter = TEXT("KnobA1 Main");
	Node->AllocateDefaultPins();

	TestNotNull(TEXT("exec in"), FindPin(Node, UEdGraphSchema_K2::PN_Execute, EGPD_Input));
	TestNotNull(TEXT("exec out"), FindPin(Node, UEdGraphSchema_K2::PN_Then, EGPD_Output));

	UEdGraphPin* TablePin = FindPin(Node, FName(TEXT("AddressTable")), EGPD_Input);
	TestNotNull(TEXT("AddressTable input pin"), TablePin);
	if (TablePin)
	{
		TestEqual(TEXT("table pin is object"), TablePin->PinType.PinCategory, UEdGraphSchema_K2::PC_Object);
		TestEqual(TEXT("table pin default = Details value"), Cast<UDataTable>(TablePin->DefaultObject.Get()), DT);
	}

	UEdGraphPin* FilterPin = FindPin(Node, FName(TEXT("AddressFilter")), EGPD_Input);
	TestNotNull(TEXT("AddressFilter input pin"), FilterPin);
	if (FilterPin)
	{
		TestEqual(TEXT("filter pin is string"), FilterPin->PinType.PinCategory, UEdGraphSchema_K2::PC_String);
		TestEqual(TEXT("filter pin default = Details value"), FilterPin->DefaultValue, FString(TEXT("KnobA1 Main")));
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
