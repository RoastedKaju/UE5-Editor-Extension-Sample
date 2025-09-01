#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(EditorToolbarSample, Log, All);

class FEditorToolbarSampleModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

protected:
    void RegisterToolbarExtension();

private:
    FNewToolMenuChoice CreateComboChoices();
    void ShowNotification();
    void ShowDialogBox();
    void ShowCompoundWidget();
};
