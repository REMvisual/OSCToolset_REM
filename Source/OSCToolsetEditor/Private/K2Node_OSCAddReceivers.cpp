#include "K2Node_OSCAddReceivers.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Self.h"
#include "KismetCompiler.h"
#include "Styling/AppStyle.h"

#include "Functions/OSCT_Manager.h" // UOSCT_Manager::AddReceiversFromDataTable

#define LOCTEXT_NAMESPACE "K2Node_OSCAddReceivers"

namespace
{
	// Input data-pin names (override the Details-panel properties when connected).
	static const FName NAME_AddressTable(TEXT("AddressTable"));
	static const FName NAME_AddressFilter(TEXT("AddressFilter"));
}

UK2Node_OSCAddReceivers::UK2Node_OSCAddReceivers(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_OSCAddReceivers::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Optional input: Address Table. Defaults from the AddressTable property; overrides it when connected.
	{
		FEdGraphPinType TableType;
		TableType.PinCategory = UEdGraphSchema_K2::PC_Object;
		TableType.PinSubCategoryObject = UDataTable::StaticClass();
		UEdGraphPin* TablePin = CreatePin(EGPD_Input, TableType, NAME_AddressTable);
		TablePin->DefaultObject = AddressTable;
		TablePin->PinFriendlyName = LOCTEXT("AddressTablePin", "Address Table");
	}

	// Optional input: Address Filter. Defaults from the AddressFilter property; overrides it when connected.
	{
		UEdGraphPin* FilterPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, NAME_AddressFilter);
		FilterPin->DefaultValue = AddressFilter;
		FilterPin->PinFriendlyName = LOCTEXT("AddressFilterPin", "Address Filter");
	}

	Super::AllocateDefaultPins();
}

UEdGraphPin* UK2Node_OSCAddReceivers::GetExecPin() const
{
	return FindPin(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
}
UEdGraphPin* UK2Node_OSCAddReceivers::GetThenPin() const
{
	return FindPin(UEdGraphSchema_K2::PN_Then, EGPD_Output);
}
UEdGraphPin* UK2Node_OSCAddReceivers::GetTablePin() const
{
	return FindPin(NAME_AddressTable, EGPD_Input);
}
UEdGraphPin* UK2Node_OSCAddReceivers::GetFilterPin() const
{
	return FindPin(NAME_AddressFilter, EGPD_Input);
}

void UK2Node_OSCAddReceivers::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	// Self for WorldContext (Owner resolves via DefaultToSelf).
	UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
	SelfNode->AllocateDefaultPins();
	UEdGraphPin* SelfPin = SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);

	UK2Node_CallFunction* Call = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	// Referenced by name: AddReceiversFromDataTable is a private (but BlueprintCallable) member, so
	// GET_FUNCTION_NAME_CHECKED can't take its address from here — reflection resolves it fine by name.
	Call->FunctionReference.SetExternalMember(
		FName(TEXT("AddReceiversFromDataTable")), UOSCT_Manager::StaticClass());
	Call->AllocateDefaultPins();

	if (UEdGraphPin* WorldPin = Call->FindPin(TEXT("WorldContextObject"))) Schema->TryCreateConnection(SelfPin, WorldPin);

	// InTable: if our AddressTable pin is connected, forward the link; otherwise copy the pin's own default.
	if (UEdGraphPin* CallInTable = Call->FindPin(TEXT("InTable")))
	{
		UEdGraphPin* MyTablePin = GetTablePin();
		if (MyTablePin && MyTablePin->LinkedTo.Num() > 0)
		{
			CompilerContext.MovePinLinksToIntermediate(*MyTablePin, *CallInTable);
		}
		else
		{
			CallInTable->DefaultObject = MyTablePin ? MyTablePin->DefaultObject : ToRawPtr(AddressTable);
		}
	}

	// AddressFilter: if our AddressFilter pin is connected, forward the link; otherwise copy the pin's own default.
	if (UEdGraphPin* CallFilter = Call->FindPin(TEXT("AddressFilter")))
	{
		UEdGraphPin* MyFilterPin = GetFilterPin();
		if (MyFilterPin && MyFilterPin->LinkedTo.Num() > 0)
		{
			CompilerContext.MovePinLinksToIntermediate(*MyFilterPin, *CallFilter);
		}
		else
		{
			CallFilter->DefaultValue = MyFilterPin ? MyFilterPin->DefaultValue : AddressFilter;
		}
	}

	// Splice exec.
	UEdGraphPin* CallExec = Call->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* CallThen = Call->FindPinChecked(UEdGraphSchema_K2::PN_Then);
	if (UEdGraphPin* MyExec = GetExecPin()) CompilerContext.MovePinLinksToIntermediate(*MyExec, *CallExec);
	if (UEdGraphPin* MyThen = GetThenPin()) CompilerContext.MovePinLinksToIntermediate(*MyThen, *CallThen);

	BreakAllNodeLinks();
}

#if WITH_EDITOR
void UK2Node_OSCAddReceivers::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropName = PropertyChangedEvent.GetPropertyName();
	bool bChanged = false;

	if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCAddReceivers, AddressTable))
	{
		if (UEdGraphPin* TablePin = GetTablePin())
		{
			TablePin->DefaultObject = AddressTable;
			bChanged = true;
		}
	}
	else if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCAddReceivers, AddressFilter))
	{
		if (UEdGraphPin* FilterPin = GetFilterPin())
		{
			FilterPin->DefaultValue = AddressFilter;
			bChanged = true;
		}
	}

	if (bChanged)
	{
		if (UEdGraph* Graph = GetGraph())
		{
			Graph->NotifyGraphChanged();
		}
	}
}
#endif

FText UK2Node_OSCAddReceivers::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "Add Receivers From Data Table");
}

FText UK2Node_OSCAddReceivers::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Registers OSC receivers for this actor from a DataTable (with optional Address filter). Set Table + Filter in Details; drive from BeginPlay.");
}

FText UK2Node_OSCAddReceivers::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "OSCToolset");
}

FSlateIcon UK2Node_OSCAddReceivers::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = FLinearColor(0.0f, 0.78f, 1.0f);
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_OSCAddReceivers::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		check(Spawner);
		ActionRegistrar.AddBlueprintAction(ActionKey, Spawner);
	}
}

#undef LOCTEXT_NAMESPACE
