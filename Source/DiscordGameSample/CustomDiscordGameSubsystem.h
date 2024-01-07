// Copyright (c) 2024 xist.gg

#pragma once

#include "CoreMinimal.h"
#include "DiscordGame/DiscordGameSubsystem.h"
#include "CustomDiscordGameSubsystem.generated.h"

/**
 * Custom Discord Game Subsystem
 *
 * This is an example of how you can customize the base UDiscordGameSubsystem
 * to add your own functionality using the Discord GameSDK.
 */
UCLASS()
class DISCORDGAMESAMPLE_API UCustomDiscordGameSubsystem : public UDiscordGameSubsystem
{
	GENERATED_BODY()

public:
	// Set Class Defaults
	UCustomDiscordGameSubsystem();

	//~UDiscordGameSubsystem interface
	virtual void NativeOnDiscordCoreCreated() override;
	virtual void NativeOnDiscordCoreReset() override;
	//~End of UDiscordGameSubsystem interface

	bool UpdateActivity();
	void ClearActivity();

};
