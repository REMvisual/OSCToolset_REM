#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "Engine/DataTable.h"
#include "K2Node_SwitchOnDataTable.generated.h"

class UEdGraphPin;
class FKismetCompilerContext;

/**
 * Switches on a String, with case pins auto-populated from a column in a DataTable.
 * Set DataTable in the details panel. Leave ColumnName empty to use Row Names as pin values,
 * or set it to a column name (e.g. "Address") to use that column's values.
 */
UCLASS(MinimalAPI, meta=(Keywords="switch datatable string"))
class UK2Node_SwitchOnDataTable : public UK2Node_Switch
{
	GENERATED_BODY()

public:
	UK2Node_SwitchOnDataTable(const FObjectInitializer& ObjectInitializer);

	/** DataTable asset whose rows drive the switch case pins. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PinOptions")
	TObjectPtr<UDataTable> DataTable;

	/** Column to read pin values from. Empty = use Row Names. Defaults to "Address". */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PinOptions")
	FName ColumnName;

	/** Optional case-insensitive filter. Empty = include all rows.
	 *  Space-separated terms are OR-ed: "KnobA1 KnobA2" matches either.
	 *  Use * as a wildcard for partial matches: "KnobA1*" matches KnobA1, KnobA10, KnobA11...
	 *  Without *, a term must match the column value EXACTLY (case-insensitive). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PinOptions")
	FString RowFilter;

	/** If true, the switch compare is case-sensitive. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PinOptions")
	bool bIsCaseSensitive;

	//~ UObject
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//~ UEdGraphNode
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void PostPasteNode() override;
	virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }

	//~ UK2Node
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;

	//~ UK2Node_Switch
	virtual FEdGraphPinType GetPinType() const override;
	virtual FName GetPinNameGivenIndex(int32 Index) const override;
	virtual FName GetUniquePinName() override;
	virtual void AddPinToSwitchNode() override;
	virtual FString GetExportTextForPin(const UEdGraphPin* Pin) const override;

	/** Re-reads the DataTable + column and regenerates case pin names cache. */
	void RefreshCaseValues();

	/** Cached pin values (one per case pin). Refreshed when DataTable/ColumnName changes. */
	UPROPERTY()
	TArray<FName> PinNames;

protected:
	virtual void CreateCasePins() override;
	virtual void CreateSelectionPin() override;

private:
	/** Pure helper: read pin values from a DataTable. Tested in isolation. */
	static TArray<FName> ExtractPinValues(const UDataTable* Table, FName Column);

	/** Bound to UDataTable::OnDataTableChanged. */
	void HandleDataTableChanged();

	/** Rebinds the OnDataTableChanged delegate, removing the old binding if DataTable was reassigned. */
	void RebindDataTableHandler();

	bool bDataTableChangeHandlerBound;

	/** Tracks which DataTable we last bound to, so we can unbind on reassignment. */
	UPROPERTY(Transient)
	TObjectPtr<UDataTable> BoundDataTable;
};
