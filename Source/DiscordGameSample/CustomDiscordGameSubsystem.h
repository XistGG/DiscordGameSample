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
	/** Get this subsystem, if the Engine exists, else nullptr */
	static UCustomDiscordGameSubsystem* Get() { return Cast<UCustomDiscordGameSubsystem>(UDiscordGameSubsystem::Get()); }

	/** Get this subsystem, making SURE we got one. */
	static UCustomDiscordGameSubsystem* GetChecked()
	{
		UCustomDiscordGameSubsystem* Subsystem = Get();
		check(Subsystem);
		return Subsystem;
	}

	// Set Class Defaults
	UCustomDiscordGameSubsystem();

	//~UDiscordGameSubsystem interface
	virtual void NativeOnDiscordCoreCreated() override;
	virtual void NativeOnDiscordCoreReset() override;
	//~End of UDiscordGameSubsystem interface

	/** Update Discord's Rich Presence Activity info */
	bool UpdateActivity();

#if false
	// ClearActivity DOES NOT WORK as of Discord GameSDK 3.2.1
	// @see https://github.com/discord/discord-api-docs/issues/6612
	void ClearActivity();
#endif

};
