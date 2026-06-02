#include "K2Node_SwitchOnDataTable.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/DataTable.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"
#include "Kismet/KismetStringLibrary.h"
#include "Styling/AppStyle.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "K2Node_SwitchOnDataTable"

namespace
{
	const FName SelectionPinName(TEXT("Selection"));
}

UK2Node_SwitchOnDataTable::UK2Node_SwitchOnDataTable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ColumnName(TEXT("Address"))
	, bIsCaseSensitive(false)
	, bDataTableChangeHandlerBound(false)
{
	bHasDefaultPin = true;
	FunctionName = TEXT("NotEqual_StriStri");
	FunctionClass = UKismetStringLibrary::StaticClass();
}

void UK2Node_SwitchOnDataTable::RebindDataTableHandler()
{
	if (BoundDataTable && BoundDataTable != DataTable)
	{
		BoundDataTable->OnDataTableChanged().RemoveAll(this);
		BoundDataTable = nullptr;
		bDataTableChangeHandlerBound = false;
	}
	if (DataTable && !bDataTableChangeHandlerBound)
	{
		DataTable->OnDataTableChanged().AddUObject(this, &UK2Node_SwitchOnDataTable::HandleDataTableChanged);
		BoundDataTable = DataTable;
		bDataTableChangeHandlerBound = true;
	}
}

void UK2Node_SwitchOnDataTable::PostInitProperties()
{
	Super::PostInitProperties();
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad))
	{
		RebindDataTableHandler();
	}
}

void UK2Node_SwitchOnDataTable::PostLoad()
{
	Super::PostLoad();
	RefreshCaseValues();
	RebindDataTableHandler();
}

void UK2Node_SwitchOnDataTable::BeginDestroy()
{
	if (BoundDataTable && bDataTableChangeHandlerBound)
	{
		BoundDataTable->OnDataTableChanged().RemoveAll(this);
		BoundDataTable = nullptr;
		bDataTableChangeHandlerBound = false;
	}
	Super::BeginDestroy();
}

#if WITH_EDITOR
void UK2Node_SwitchOnDataTable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropName = PropertyChangedEvent.GetPropertyName();
	if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchOnDataTable, DataTable) ||
	    PropName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchOnDataTable, ColumnName) ||
	    PropName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchOnDataTable, RowFilter) ||
	    PropName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchOnDataTable, bIsCaseSensitive) ||
	    PropName == GET_MEMBER_NAME_CHECKED(UK2Node_Switch, bHasDefaultPin))
	{
		if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchOnDataTable, bIsCaseSensitive))
		{
			FunctionName = bIsCaseSensitive ? TEXT("NotEqual_StrStr") : TEXT("NotEqual_StriStri");
		}

		if (PropName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchOnDataTable, DataTable))
		{
			RebindDataTableHandler();
		}

		RefreshCaseValues();
		ReconstructNode();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FText UK2Node_SwitchOnDataTable::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (DataTable)
	{
		return FText::Format(LOCTEXT("NodeTitleWithDT", "Switch on DataTable ({0})"),
		                     FText::FromString(DataTable->GetName()));
	}
	return LOCTEXT("NodeTitle", "Switch on DataTable");
}

FText UK2Node_SwitchOnDataTable::GetTooltipText() const
{
	return LOCTEXT("Tooltip",
		"Selects an execution pin based on a string input. Case pins are auto-generated "
		"from the DataTable's rows. Set DataTable + ColumnName in the details panel.");
}

FSlateIcon UK2Node_SwitchOnDataTable::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = FLinearColor::White;
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "GraphEditor.Switch_16x");
	return Icon;
}

void UK2Node_SwitchOnDataTable::PostPasteNode()
{
	Super::PostPasteNode();
	RefreshCaseValues();
	RebindDataTableHandler();
}

void UK2Node_SwitchOnDataTable::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	if (!DataTable)
	{
		MessageLog.Warning(*LOCTEXT("NoDataTable", "Switch on DataTable @@ has no DataTable assigned — no case pins will fire.").ToString(), this);
		return;
	}

	if (!ColumnName.IsNone())
	{
		const UScriptStruct* RowStruct = DataTable->GetRowStruct();
		FProperty* Found = RowStruct ? RowStruct->FindPropertyByName(ColumnName) : nullptr;
		if (RowStruct && !Found)
		{
			for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
			{
				if (It->GetFName().ToString().Equals(ColumnName.ToString(), ESearchCase::IgnoreCase))
				{
					Found = *It;
					break;
				}
			}
		}
		if (!Found)
		{
			MessageLog.Error(*FText::Format(LOCTEXT("BadColumn", "Switch on DataTable @@ ColumnName '{0}' does not exist on row struct."),
				FText::FromName(ColumnName)).ToString(), this);
			return;
		}
	}

	TSet<FName> Seen;
	for (const FName& Name : PinNames)
	{
		bool bAlreadyIn = false;
		Seen.Add(Name, &bAlreadyIn);
		if (bAlreadyIn)
		{
			MessageLog.Warning(*FText::Format(LOCTEXT("DupPin", "Switch on DataTable @@ has duplicate case pin '{0}' — only the first will be reachable."),
				FText::FromName(Name)).ToString(), this);
		}
	}
}

void UK2Node_SwitchOnDataTable::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_SwitchOnDataTable::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Utilities|Flow Control");
}

FEdGraphPinType UK2Node_SwitchOnDataTable::GetPinType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_String;
	return PinType;
}

FName UK2Node_SwitchOnDataTable::GetPinNameGivenIndex(int32 Index) const
{
	if (PinNames.IsValidIndex(Index))
	{
		return PinNames[Index];
	}
	return NAME_None;
}

FName UK2Node_SwitchOnDataTable::GetUniquePinName()
{
	FName NewName;
	int32 Suffix = PinNames.Num();
	do
	{
		NewName = *FString::Printf(TEXT("Case_%d"), Suffix++);
	} while (PinNames.Contains(NewName));
	return NewName;
}

void UK2Node_SwitchOnDataTable::AddPinToSwitchNode()
{
	const FName NewPinName = GetUniquePinName();
	PinNames.Add(NewPinName);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, NewPinName);
}

FString UK2Node_SwitchOnDataTable::GetExportTextForPin(const UEdGraphPin* Pin) const
{
	return Pin ? Pin->PinName.ToString() : FString();
}

void UK2Node_SwitchOnDataTable::CreateSelectionPin()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, SelectionPinName);
}

void UK2Node_SwitchOnDataTable::CreateCasePins()
{
	for (const FName& PinName : PinNames)
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
	}
}

void UK2Node_SwitchOnDataTable::RefreshCaseValues()
{
	PinNames.Reset();
	const TArray<FName> Extracted = ExtractPinValues(DataTable, ColumnName);

	TArray<FString> Patterns;
	RowFilter.ParseIntoArrayWS(Patterns);

	if (Patterns.Num() == 0)
	{
		PinNames.Append(Extracted);
		return;
	}

	for (const FName& Value : Extracted)
	{
		const FString ValueStr = Value.ToString();
		for (const FString& Pattern : Patterns)
		{
			const bool bMatches = Pattern.Contains(TEXT("*")) || Pattern.Contains(TEXT("?"))
				? ValueStr.MatchesWildcard(Pattern, ESearchCase::IgnoreCase)
				: ValueStr.Equals(Pattern, ESearchCase::IgnoreCase);
			if (bMatches)
			{
				PinNames.Add(Value);
				break;
			}
		}
	}
}

TArray<FName> UK2Node_SwitchOnDataTable::ExtractPinValues(const UDataTable* Table, FName Column)
{
	TArray<FName> Out;
	if (!Table)
	{
		return Out;
	}

	if (Column.IsNone())
	{
		for (const TPair<FName, uint8*>& Row : Table->GetRowMap())
		{
			Out.Add(Row.Key);
		}
		return Out;
	}

	const UScriptStruct* RowStruct = Table->GetRowStruct();
	if (!RowStruct)
	{
		return Out;
	}

	FProperty* Prop = RowStruct->FindPropertyByName(Column);
	if (!Prop)
	{
		for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
		{
			if (It->GetFName().ToString().Equals(Column.ToString(), ESearchCase::IgnoreCase))
			{
				Prop = *It;
				break;
			}
		}
	}
	if (!Prop)
	{
		return Out;
	}

	for (const TPair<FName, uint8*>& Row : Table->GetRowMap())
	{
		FString Value;
		const void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Row.Value);
		Prop->ExportText_Direct(Value, ValuePtr, nullptr, nullptr, PPF_None);
		Value.RemoveFromStart(TEXT("\""));
		Value.RemoveFromEnd(TEXT("\""));
		if (!Value.IsEmpty())
		{
			Out.Add(FName(*Value));
		}
	}
	return Out;
}

void UK2Node_SwitchOnDataTable::HandleDataTableChanged()
{
	RefreshCaseValues();
	ReconstructNode();
}

#undef LOCTEXT_NAMESPACE
