// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExtendEditorManager : ModuleRules
{
	public ExtendEditorManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Blutility",
				"UMG",
				"UMGEditor",
				"EditorScriptingUtilities",
				"UnrealEd",
				"AssetTools",
				"ContentBrowser",
				"Projects",
				"NetCore"
			}
			);
	}
}
