#include "K2Node_OSCSwitchValues.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_Self.h"
#include "KismetCompiler.h"
#include "Kismet/KismetStringLibrary.h"
#include "Styling/AppStyle.h"

#include "OSCT_Modules.h"              // FOSCT_Receiver, FOSCT_ReceiverRow
#include "Functions/OSCT_Manager.h"   // UOSCT_Manager getters
#include "Functions/OSCT_Functions.h" // AddressPassesFilter, GetReceiverAddress

#define LOCTEXT_NAMESPACE "K2Node_OSCSwitchValues"

const FName UK2Node_OSCSwitchValues::ReceiverPinName(TEXT("Receiver"));
const FName UK2Node_OSCSwitchValues::DefaultPinName(TEXT("Default"));

UK2Node_OSCSwitchValues::UK2Node_OSCSwitchValues(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIncludeDefault = true;
}

FName UK2Node_OSCSwitchValues::FunctionNameForType(EOSCT_ModuleType Type)
{
	switch (Type)
	{
	case EOSCT_ModuleType::FLOAT:     return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCFloat);
	case EOSCT_ModuleType::INT:       return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCInteger);
	case EOSCT_ModuleType::VEC2:      return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCVector2);
	case EOSCT_ModuleType::VEC3:      return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCVector3);
	case EOSCT_ModuleType::ROTATION:  return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCRotation);
	case EOSCT_ModuleType::COLOR:     return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCColor);
	case EOSCT_ModuleType::TRANSFORM: return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCTransform);
	case EOSCT_ModuleType::BOOL:      return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCBool);
	case EOSCT_ModuleType::STRING:    return GET_FUNCTION_NAME_CHECKED(UOSCT_Manager, GetOSCString);
	default:                          return NAME_None; // EVENT / NOTE = exec only
	}
}

bool UK2Node_OSCSwitchValues::MakePinType(EOSCT_ModuleType Type, FEdGraphPinType& OutType)
{
	OutType = FEdGraphPinType();
	switch (Type)
	{
	case EOSCT_ModuleType::FLOAT:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Real;
		OutType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
		return true;
	case EOSCT_ModuleType::INT:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Int;
		return true;
	case EOSCT_ModuleType::BOOL:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		return true;
	case EOSCT_ModuleType::STRING:
		OutType.PinCategory = UEdGraphSchema_K2::PC_String;
		return true;
	case EOSCT_ModuleType::VEC2:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutType.PinSubCategoryObject = TBaseStructure<FVector2D>::Get();
		return true;
	case EOSCT_ModuleType::VEC3:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
		return true;
	case EOSCT_ModuleType::ROTATION:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
		return true;
	case EOSCT_ModuleType::COLOR:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutType.PinSubCategoryObject = TBaseStructure<FLinearColor>::Get();
		return true;
	case EOSCT_ModuleType::TRANSFORM:
		OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutType.PinSubCategoryObject = TBaseStructure<FTransform>::Get();
		return true;
	default:
		return false; // EVENT / NOTE — exec only, no value pin
	}
}

void UK2Node_OSCSwitchValues::CollectEntries(TArray<FOSCSwitchEntry>& Out) const
{
	TSet<FString> Seen;

	auto TryAdd = [&](const FString& Address, EOSCT_ModuleType Type)
	{
		if (Address.IsEmpty()) return;
		const FString Lower = Address.ToLower();
		if (Seen.Contains(Lower)) return;
		Seen.Add(Lower);

		FEdGraphPinType Dummy;
		FOSCSwitchEntry Entry;
		Entry.Address = Address;
		Entry.Type = Type;
		Entry.ExecPinName = FName(*Address);
		Entry.bHasValue = MakePinType(Type, Dummy);
		Entry.ValuePinName = Entry.bHasValue ? FName(*(Address + TEXT("_Value"))) : NAME_None;
		Out.Add(Entry);
	};

	if (AddressTable && AddressTable->RowStruct && AddressTable->RowStruct->IsChildOf(FOSCT_ReceiverRow::StaticStruct()))
	{
		static const FString Context(TEXT("OSCSwitchValues Rows"));
		TArray<FOSCT_ReceiverRow*> Rows;
		AddressTable->GetAllRows<FOSCT_ReceiverRow>(Context, Rows);
		for (const FOSCT_ReceiverRow* Row : Rows)
		{
			if (Row && UOSCT_Functions::AddressPassesFilter(Row->Address, AddressFilter))
			{
				TryAdd(Row->Address, Row->ModuleType);
			}
		}
	}

	for (const FOSCT_NodeAddress& A : Addresses)
	{
		TryAdd(A.Address, A.Type);
	}
}

void UK2Node_OSCSwitchValues::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// Receiver input (from any GET_* event)
	FEdGraphPinType RecvType;
	RecvType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	RecvType.PinSubCategoryObject = FOSCT_Receiver::StaticStruct();
	CreatePin(EGPD_Input, RecvType, ReceiverPinName);

	// Note: Address Table + Filter live in the Details panel (they DEFINE the case pins at compile time, like
	// the DataTable Switch and engine Switch-on-Enum nodes — so they can't be runtime pins).

	// Default exec output (optional)
	if (bIncludeDefault)
	{
		UEdGraphPin* DefaultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, DefaultPinName);
		DefaultPin->PinFriendlyName = LOCTEXT("DefaultPin", "Default");
	}

	TArray<FOSCSwitchEntry> Entries;
	CollectEntries(Entries);
	for (const FOSCSwitchEntry& Entry : Entries)
	{
		UEdGraphPin* ExecOut = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, Entry.ExecPinName);
		ExecOut->PinFriendlyName = FText::FromString(Entry.Address);

		if (Entry.bHasValue)
		{
			FEdGraphPinType ValType;
			MakePinType(Entry.Type, ValType);
			UEdGraphPin* ValOut = CreatePin(EGPD_Output, ValType, Entry.ValuePinName);
			ValOut->PinFriendlyName = FText::FromString(FString::Printf(TEXT("%s Value"), *Entry.Address));
		}
	}

	Super::AllocateDefaultPins();
}

UEdGraphPin* UK2Node_OSCSwitchValues::GetExecPin() const
{
	return FindPin(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
}
UEdGraphPin* UK2Node_OSCSwitchValues::GetReceiverPin() const
{
	return FindPin(ReceiverPinName, EGPD_Input);
}
UEdGraphPin* UK2Node_OSCSwitchValues::GetDefaultPin() const
{
	return FindPin(DefaultPinName, EGPD_Output);
}

void UK2Node_OSCSwitchValues::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	TArray<FOSCSwitchEntry> Entries;
	CollectEntries(Entries);
	if (Entries.Num() == 0)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("NoAddresses", "@@ has no addresses — add an Address or an AddressTable.").ToString(), this);
		BreakAllNodeLinks();
		return;
	}

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	// Self (WorldContext source for the getters)
	UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
	SelfNode->AllocateDefaultPins();
	UEdGraphPin* SelfPin = SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);

	// The Receiver pin is OPTIONAL: it only drives exec routing. Value polling + auto-register run
	// regardless, so the node also works as a pure value reader (drive from Tick, Receiver unconnected).
	const bool bHasReceiver = (GetReceiverPin() && GetReceiverPin()->LinkedTo.Num() > 0);

	// --- Value polling chain (populate every value pin; lazy auto-registers) — ALWAYS runs ---
	UEdGraphPin* ChainExecIn = nullptr;
	UEdGraphPin* ChainThen = nullptr;
	for (const FOSCSwitchEntry& Entry : Entries)
	{
		if (!Entry.bHasValue) continue;
		const FName Func = FunctionNameForType(Entry.Type);
		if (Func == NAME_None) continue;

		UK2Node_CallFunction* Call = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		Call->FunctionReference.SetExternalMember(Func, UOSCT_Manager::StaticClass());
		Call->AllocateDefaultPins();

		if (UEdGraphPin* AddrPin = Call->FindPin(TEXT("Address"))) AddrPin->DefaultValue = Entry.Address;
		if (UEdGraphPin* WorldPin = Call->FindPin(TEXT("WorldContextObject"))) Schema->TryCreateConnection(SelfPin, WorldPin);
		if (UEdGraphPin* OutValuePin = Call->FindPin(TEXT("OutValue")))
		{
			if (UEdGraphPin* MyValuePin = FindPin(Entry.ValuePinName, EGPD_Output))
			{
				CompilerContext.MovePinLinksToIntermediate(*MyValuePin, *OutValuePin);
			}
		}

		UEdGraphPin* CallExec = Call->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
		UEdGraphPin* CallThen = Call->FindPinChecked(UEdGraphSchema_K2::PN_Then);
		if (!ChainExecIn) ChainExecIn = CallExec;
		if (ChainThen) Schema->TryCreateConnection(ChainThen, CallExec);
		ChainThen = CallThen;
	}

	UEdGraphPin* MyExec = GetExecPin();

	if (bHasReceiver)
	{
		// Extract incoming address string from the Receiver (pure).
		UK2Node_CallFunction* GetAddr = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		GetAddr->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UOSCT_Functions, GetReceiverAddress), UOSCT_Functions::StaticClass());
		GetAddr->AllocateDefaultPins();
		CompilerContext.MovePinLinksToIntermediate(*GetReceiverPin(), *GetAddr->FindPinChecked(TEXT("Receiver")));
		UEdGraphPin* IncomingAddrPin = GetAddr->GetReturnValuePin();

		// --- Routing chain (branch per address on the incoming address) ---
		UEdGraphPin* RouteExecIn = nullptr;
		UEdGraphPin* PrevElse = nullptr;
		for (const FOSCSwitchEntry& Entry : Entries)
		{
			UK2Node_CallFunction* Eq = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
			Eq->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetStringLibrary, EqualEqual_StriStri), UKismetStringLibrary::StaticClass());
			Eq->AllocateDefaultPins();
			Schema->TryCreateConnection(IncomingAddrPin, Eq->FindPinChecked(TEXT("A")));
			Eq->FindPinChecked(TEXT("B"))->DefaultValue = Entry.Address;
			UEdGraphPin* EqResult = Eq->GetReturnValuePin();

			UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
			Branch->AllocateDefaultPins();
			Schema->TryCreateConnection(EqResult, Branch->GetConditionPin());

			// matching address fires its exec output
			if (UEdGraphPin* EntryExecOut = FindPin(Entry.ExecPinName, EGPD_Output))
			{
				CompilerContext.MovePinLinksToIntermediate(*EntryExecOut, *Branch->GetThenPin());
			}

			UEdGraphPin* BranchExec = Branch->GetExecPin();
			if (!RouteExecIn) RouteExecIn = BranchExec;
			if (PrevElse) Schema->TryCreateConnection(PrevElse, BranchExec);
			PrevElse = Branch->GetElsePin();
		}
		// No-match -> Default (only if the Default pin is shown; otherwise exec simply stops on no match)
		if (PrevElse && bIncludeDefault)
		{
			if (UEdGraphPin* DefPin = GetDefaultPin())
			{
				CompilerContext.MovePinLinksToIntermediate(*DefPin, *PrevElse);
			}
		}

		// Splice exec: incoming exec -> value chain -> routing.
		UEdGraphPin* FirstExec = ChainExecIn ? ChainExecIn : RouteExecIn;
		if (MyExec && FirstExec) CompilerContext.MovePinLinksToIntermediate(*MyExec, *FirstExec);
		if (ChainThen && RouteExecIn) Schema->TryCreateConnection(ChainThen, RouteExecIn);
	}
	else
	{
		// No Receiver wired -> pure value reader (e.g. Tick): exec -> value polls -> Default.
		// (Per-address exec outputs cannot fire without a Receiver to route on.)
		if (MyExec && ChainExecIn)
		{
			CompilerContext.MovePinLinksToIntermediate(*MyExec, *ChainExecIn);
			if (ChainThen && bIncludeDefault)
			{
				if (UEdGraphPin* DefPin = GetDefaultPin())
				{
					CompilerContext.MovePinLinksToIntermediate(*DefPin, *ChainThen);
				}
			}
		}
	}

	BreakAllNodeLinks();
}

void UK2Node_OSCSwitchValues::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	TArray<FOSCSwitchEntry> Entries;
	CollectEntries(Entries);
	if (Entries.Num() == 0)
	{
		MessageLog.Error(*LOCTEXT("ValidateNoAddresses", "@@ has no addresses — add an Address or an AddressTable.").ToString(), this);
	}
}

#if WITH_EDITOR
void UK2Node_OSCSwitchValues::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropName = PropertyChangedEvent.GetPropertyName();
	if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCSwitchValues, Addresses) ||
		PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCSwitchValues, AddressTable) ||
		PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCSwitchValues, AddressFilter) ||
		PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCSwitchValues, bIncludeDefault) ||
		PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UK2Node_OSCSwitchValues, Addresses))
	{
		ReconstructNode();
	}
}
#endif

FText UK2Node_OSCSwitchValues::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "OSC Switch Values");
}

FText UK2Node_OSCSwitchValues::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Drive from any GET_* OSCT Router event (wire its Receiver in). Routes exec to the matching address AND exposes each address's live value as a pin. Auto-registers listed addresses.");
}

FText UK2Node_OSCSwitchValues::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "OSCToolset|Get");
}

FSlateIcon UK2Node_OSCSwitchValues::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = FLinearColor(1.0f, 0.0f, 0.39f); // EVENT debug pink-ish
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "GraphEditor.Switch_16x");
	return Icon;
}

void UK2Node_OSCSwitchValues::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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
