#pragma once
#include "Engine.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "PropertyEditor/Public/PropertyEditorModule.h"
#include "ToolEditor/Customization.h"

class FToolEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};