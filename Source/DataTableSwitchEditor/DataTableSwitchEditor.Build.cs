using UnrealBuildTool;

public class DataTableSwitchEditor : ModuleRules
{
	public DataTableSwitchEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
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
