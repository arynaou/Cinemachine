#include "Customization.h"
//#include "ToolEditor.h"
#include "Cinemachine/VirtualCamera.h"
#include "PropertyEditing.h"

#define LOCTEXT_NAMESPACE "EditorModule"

TSharedRef< IDetailCustomization > FCustomization::MakeInstance()
{
    return MakeShareable(new FCustomization);
}

void FCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    //
}

#undef LOCTEXT_NAMESPACE