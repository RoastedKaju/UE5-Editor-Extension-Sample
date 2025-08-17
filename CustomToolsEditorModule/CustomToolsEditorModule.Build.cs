using UnrealBuildTool;

public class CustomToolsEditorModule : ModuleRules
{
    public CustomToolsEditorModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "ToolMenus" });
    }
}