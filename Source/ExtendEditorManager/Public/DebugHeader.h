#pragma once

#include "Misc/MessageDialog.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace DebugHelper
{
	static void Print(const FString& Message, const FColor& Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
		}
	}

	static void PrintLog(const FString& Message)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	}

	static EAppReturnType::Type ShowDialogMessage(EAppMsgType::Type Type, const FString& Message, bool bShowAsWarning)
	{
		if (bShowAsWarning)
		{
			return FMessageDialog::Open(Type, FText::FromString(Message), FText::FromString("Warning"));
		}

		return FMessageDialog::Open(Type, FText::FromString(Message));
	}

	static void ShowNotification(const FString& Message)
	{
		FNotificationInfo Notification(FText::FromString(Message));
		Notification.bUseLargeFont = true;
		Notification.ExpireDuration = 10.0f;

		FSlateNotificationManager::Get().AddNotification(Notification);
	}
}
