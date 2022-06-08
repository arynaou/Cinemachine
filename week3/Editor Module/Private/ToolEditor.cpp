#include "ToolEditor.h"

IMPLEMENT_GAME_MODULE(FToolEditorModule, ToolEditor)

void FToolEditorModule::StartupModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");

    // Register our customization to be used by a class 'UMyClass' or 'AMyClass'. Note the prefix must be dropped.
    PropertyModule.RegisterCustomClassLayout(
        "CustomTool",
        FOnGetDetailCustomizationInstance::CreateStatic(&FCustomization::MakeInstance)
    );

    PropertyModule.NotifyCustomizationModuleChanged();
}

void FToolEditorModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

        PropertyModule.UnregisterCustomClassLayout("CustomTool");
    }
}