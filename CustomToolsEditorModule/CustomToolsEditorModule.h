// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(CustomToolsEditorLog, Log, All);

class FCustomToolsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterMenus();

	// When you click on the wizard icon button
	void OnWizardButtonClicked();

	// When you click on the settings icon button
	void OnSimpleTabWindowClicked();

	// Notification list example
	void OnClickNotificationExample();
	// Dialog message box example
	void OnClickDialogMessageExample();
	// Compound Widget
	void OnClickCompoundWidgetExample();
};