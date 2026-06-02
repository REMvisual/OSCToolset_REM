#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Engine/DataTable.h"
#include "OSCT_ETypes.h"
#include "OSCToolsetEditorTypes.h"
#include "K2Node_OSCSwitchValues.generated.h"

class UEdGraphPin;
class FBlueprintActionDatabaseRegistrar;
class FKismetCompilerContext;

/**
 * Combined OSC route + read node. Drive it from any GET_* OSCT Router event (GET_Float, GET_Boolean, GET_Color, ...),
 * wiring that event's Receiver into the Receiver pin. Declare a filtered address set (typed inline, and/or an
 * FOSCT_ReceiverRow DataTable, plus a shared AddressFilter). For each address you get BOTH:
 *   - an exec output that fires when the incoming Receiver's address matches (like Switch on DataTable), and
 *   - a typed value pin carrying that address's current live value (like OSC Get Values).
 * Plus a Default exec for non-matching addresses. Register the addresses separately (Add Receivers From Data Table on BeginPlay).
 */
UCLASS(MinimalAPI, meta=(Keywords="osc switch values route receiver float bool color filter"))
class UK2Node_OSCSwitchValues : public UK2Node
{
	GENERATED_BODY()

public:
	UK2Node_OSCSwitchValues(const FObjectInitializer& ObjectInitializer);

	/** Inline addresses (each individually typed — multi-value). */
	UPROPERTY(EditAnywhere, Category="OSC")
	TArray<FOSCT_NodeAddress> Addresses;

	/** Optional FOSCT_ReceiverRow table; each row adds a case (exec + value) typed by that row's ModuleType. */
	UPROPERTY(EditAnywhere, Category="OSC")
	TObjectPtr<UDataTable> AddressTable;

	/** Shared filter applied to AddressTable rows only (inline addresses always included). Space-separated OR, '*' wildcard. */
	UPROPERTY(EditAnywhere, Category="OSC")
	FString AddressFilter;

	/** If true, show a Default exec output that fires when the incoming address matches none of the cases. */
	UPROPERTY(EditAnywhere, Category="OSC")
	bool bIncludeDefault;

	//~ UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//~ UK2Node
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual bool IsNodePure() const override { return false; }

private:
	struct FOSCSwitchEntry
	{
		FString Address;
		EOSCT_ModuleType Type;
		FName ExecPinName;   // output exec, named by address
		FName ValuePinName;  // output value pin (NAME_None for EVENT — exec only)
		bool bHasValue;
	};

	void CollectEntries(TArray<FOSCSwitchEntry>& Out) const;
	static bool MakePinType(EOSCT_ModuleType Type, FEdGraphPinType& OutType); // false = no value (EVENT)
	static FName FunctionNameForType(EOSCT_ModuleType Type);                  // NAME_None = no getter

	UEdGraphPin* GetExecPin() const;
	UEdGraphPin* GetReceiverPin() const;
	UEdGraphPin* GetDefaultPin() const;

	static const FName ReceiverPinName;
	static const FName DefaultPinName;
};
