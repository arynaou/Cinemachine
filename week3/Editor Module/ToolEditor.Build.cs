using UnrealBuildTool;

public class ToolEditor : ModuleRules
{
    public ToolEditor(ReadOnlyTargetRules Target) : base(Target)
    {
		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
                new string[]
                {
                "ToolEditor/Public"
                });

        PrivateIncludePaths.AddRange(
            new string[]
            {
            "ToolEditor/Private"
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Cinemachine", "Core", "Engine", "InputCore", "Slate", "SlateCore", "UnrealEd","PropertyEditor"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd"
            });

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            });

    }
}
