#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Engine/DataTable.h"
#include "K2Node_OSCAddReceivers.generated.h"

class UEdGraphPin;
class FBlueprintActionDatabaseRegistrar;
class FKismetCompilerContext;

/**
 * Registers OSC receivers for the owning actor from an FOSCT_ReceiverRow DataTable, with an optional address filter.
 * Same as the Add Receivers From Data Table function, but as a node with a Details panel: set the Table and Filter
 * in Details, wire exec in/out in the graph. Drive it from BeginPlay.
 */
UCLASS(MinimalAPI, meta=(Keywords="osc add receivers register data table filter"))
class UK2Node_OSCAddReceivers : public UK2Node
{
	GENERATED_BODY()

public:
	UK2Node_OSCAddReceivers(const FObjectInitializer& ObjectInitializer);

	/** FOSCT_ReceiverRow table whose rows are registered for the owner. */
	UPROPERTY(EditAnywhere, Category="OSC")
	TObjectPtr<UDataTable> AddressTable;

	/** Optional filter: only rows whose Address matches register. Space-separated OR, '*' wildcard. Empty = all rows. */
	UPROPERTY(EditAnywhere, Category="OSC")
	FString AddressFilter;

	//~ UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;

	//~ UK2Node
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual bool IsNodePure() const override { return false; }

#if WITH_EDITOR
	//~ UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UEdGraphPin* GetExecPin() const;
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetTablePin() const;
	UEdGraphPin* GetFilterPin() const;
};
