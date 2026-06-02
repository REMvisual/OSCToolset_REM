using UnrealBuildTool;

public class OSCToolsetEditor : ModuleRules
{
	public OSCToolsetEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"OSCToolset" // runtime module — for UOSCT_Manager + FOSCT_ReceiverRow + EOSCT_ModuleType
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"BlueprintGraph",
			"KismetCompiler",
			"Slate",
			"SlateCore",
			"GraphEditor",
			"EditorStyle",
			"ToolMenus"
		});
	}
}
