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

void UK2Node_OSCSwitchValues::RebindDataTableHandler()
{
	if (BoundDataTable && BoundDataTable != AddressTable)
	{
		BoundDataTable->OnDataTableChanged().RemoveAll(this);
		BoundDataTable = nullptr;
		bDataTableChangeHandlerBound = false;
	}
	if (AddressTable && !bDataTableChangeHandlerBound)
	{
		AddressTable->OnDataTableChanged().AddUObject(this, &UK2Node_OSCSwitchValues::HandleDataTableChanged);
		BoundDataTable = AddressTable;
		bDataTableChangeHandlerBound = true;
	}
}

void UK2Node_OSCSwitchValues::HandleDataTableChanged()
{
	RefreshCachedEntries();
	ReconstructNode();
}

void UK2Node_OSCSwitchValues::PostInitProperties()
{
	Super::PostInitProperties();
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad))
	{
		RebindDataTableHandler();
	}
}

void UK2Node_OSCSwitchValues::PostLoad()
{
	Super::PostLoad();
	// IMPORTANT: do NOT read AddressTable here. During load the referenced DataTable's RowStruct/rows are
	// frequently not deserialized yet (proven via logs: hasRowStruct=0, rawRows=0 on early passes), and load
	// order between this node and the table is not guaranteed. Reading it would yield zero entries and wipe the
	// snapshot, dropping the case pins / raising "has no addresses" on compile-on-load.
	//
	// The serialized CachedEntries IS the source of truth at load. If it's empty (asset saved before this field
	// existed, or wiped by the earlier buggy refresh), rebuild it from the node's own persisted case pins —
	// those are serialized with the node, so this is fully timing-independent and auto-heals old assets.
	if (CachedEntries.Num() == 0)
	{
		DeriveCachedEntriesFromPins();
	}
	RebindDataTableHandler();
}

void UK2Node_OSCSwitchValues::PostPasteNode()
{
	Super::PostPasteNode();
	RefreshCachedEntries();
	RebindDataTableHandler();
}

void UK2Node_OSCSwitchValues::BeginDestroy()
{
	if (BoundDataTable && bDataTableChangeHandlerBound)
	{
		BoundDataTable->OnDataTableChanged().RemoveAll(this);
		BoundDataTable = nullptr;
		bDataTableChangeHandlerBound = false;
	}
	Super::BeginDestroy();
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

void UK2Node_OSCSwitchValues::ResolveEntries(TArray<FOSCT_NodeAddress>& Out) const
{
	Out.Reset();
	TSet<FString> Seen;

	auto TryAdd = [&](const FString& Address, EOSCT_ModuleType Type)
	{
		if (Address.IsEmpty()) return;
		const FString Lower = Address.ToLower();
		if (Seen.Contains(Lower)) return;
		Seen.Add(Lower);

		FOSCT_NodeAddress E;
		E.Address = Address;
		E.Type = Type;
		Out.Add(E);
	};

	if (AddressTable && AddressTable->GetRowStruct() && AddressTable->GetRowStruct()->IsChildOf(FOSCT_ReceiverRow::StaticStruct()))
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

void UK2Node_OSCSwitchValues::RefreshCachedEntries()
{
	TArray<FOSCT_NodeAddress> Resolved;
	ResolveEntries(Resolved);

	if (Resolved.Num() > 0)
	{
		CachedEntries = MoveTemp(Resolved);
		return;
	}

	// Resolve came back empty. Decide whether that's authoritative or just a not-yet-loaded table:
	//  - No source configured at all (no table, no inline) -> genuinely empty, clear the cache.
	//  - Table is assigned AND readable (RowStruct valid) -> the rows/filter legitimately yield nothing, clear.
	//  - Table is assigned but NOT readable yet (RowStruct null = mid-load) -> KEEP the snapshot; wiping it
	//    here is exactly the load-order race that dropped the case pins.
	const bool bHasConfiguredSource = (AddressTable != nullptr) || (Addresses.Num() > 0);
	const bool bTableReadable = (AddressTable != nullptr) && (AddressTable->GetRowStruct() != nullptr);
	const bool bOnlyTableSource = (AddressTable != nullptr) && (Addresses.Num() == 0);

	if (!bHasConfiguredSource || bTableReadable || !bOnlyTableSource)
	{
		CachedEntries.Reset();
	}
	// else: table assigned but not loaded yet -> preserve CachedEntries
}

EOSCT_ModuleType UK2Node_OSCSwitchValues::ModuleTypeFromPinType(const FEdGraphPinType& PinType)
{
	const FName Cat = PinType.PinCategory;
	if (Cat == UEdGraphSchema_K2::PC_Real)    return EOSCT_ModuleType::FLOAT;
	if (Cat == UEdGraphSchema_K2::PC_Int)     return EOSCT_ModuleType::INT;
	if (Cat == UEdGraphSchema_K2::PC_Boolean) return EOSCT_ModuleType::BOOL;
	if (Cat == UEdGraphSchema_K2::PC_String)  return EOSCT_ModuleType::STRING;
	if (Cat == UEdGraphSchema_K2::PC_Struct)
	{
		const UScriptStruct* S = Cast<UScriptStruct>(PinType.PinSubCategoryObject.Get());
		if (S == TBaseStructure<FVector2D>::Get())    return EOSCT_ModuleType::VEC2;
		if (S == TBaseStructure<FVector>::Get())       return EOSCT_ModuleType::VEC3;
		if (S == TBaseStructure<FRotator>::Get())      return EOSCT_ModuleType::ROTATION;
		if (S == TBaseStructure<FLinearColor>::Get())  return EOSCT_ModuleType::COLOR;
		if (S == TBaseStructure<FTransform>::Get())    return EOSCT_ModuleType::TRANSFORM;
	}
	return EOSCT_ModuleType::EVENT; // exec-only (also covers NOTE)
}

void UK2Node_OSCSwitchValues::DeriveCachedEntriesFromPins()
{
	// Reconstruct the resolved address set from the node's own persisted case pins. Each address has an exec
	// output pin named after the address, and (for valued types) a sibling "<Address>_Value" pin whose type
	// recovers the EOSCT_ModuleType. This is serialized with the node, so it's available regardless of whether
	// the AddressTable asset has finished loading — the timing-independent safety net / migration path.
	TArray<FOSCT_NodeAddress> Derived;
	TSet<FString> Seen;
	for (const UEdGraphPin* Pin : Pins)
	{
		if (!Pin || Pin->Direction != EGPD_Output) continue;
		if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec) continue;
		if (Pin->PinName == DefaultPinName) continue;

		const FString Address = Pin->PinName.ToString();
		if (Address.IsEmpty()) continue;
		const FString Lower = Address.ToLower();
		if (Seen.Contains(Lower)) continue;
		Seen.Add(Lower);

		EOSCT_ModuleType Type = EOSCT_ModuleType::EVENT;
		const FName ValueName(*(Address + TEXT("_Value")));
		for (const UEdGraphPin* ValPin : Pins)
		{
			if (ValPin && ValPin->Direction == EGPD_Output && ValPin->PinName == ValueName)
			{
				Type = ModuleTypeFromPinType(ValPin->PinType);
				break;
			}
		}

		FOSCT_NodeAddress E;
		E.Address = Address;
		E.Type = Type;
		Derived.Add(E);
	}

	if (Derived.Num() > 0)
	{
		CachedEntries = MoveTemp(Derived);
	}
}

void UK2Node_OSCSwitchValues::CollectEntries(TArray<FOSCSwitchEntry>& Out) const
{
	// Build pins from the SERIALIZED snapshot so the case pins are identical across reconstruction even when
	// the AddressTable asset isn't loaded at that moment (the cause of exec links dropping on load/Play).
	// Fall back to a live resolve only when the cache is empty — freshly created nodes and pre-fix assets
	// before their first PostLoad refresh.
	TArray<FOSCT_NodeAddress> Source = CachedEntries;
	if (Source.Num() == 0)
	{
		ResolveEntries(Source);
	}

	TSet<FString> Seen;
	for (const FOSCT_NodeAddress& A : Source)
	{
		if (A.Address.IsEmpty()) continue;
		const FString Lower = A.Address.ToLower();
		if (Seen.Contains(Lower)) continue;
		Seen.Add(Lower);

		FEdGraphPinType Dummy;
		FOSCSwitchEntry Entry;
		Entry.Address = A.Address;
		Entry.Type = A.Type;
		Entry.ExecPinName = FName(*A.Address);
		Entry.bHasValue = MakePinType(A.Type, Dummy);
		Entry.ValuePinName = Entry.bHasValue ? FName(*(A.Address + TEXT("_Value"))) : NAME_None;
		Out.Add(Entry);
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
		if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_OSCSwitchValues, AddressTable))
		{
			RebindDataTableHandler();
		}
		RefreshCachedEntries();
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
