// Copyright (c) 2024 xist.gg

#include "CustomDiscordGameSubsystem.h"

UCustomDiscordGameSubsystem::UCustomDiscordGameSubsystem()
{
	// Set your own custom Discord Application ID here:
	ClientId = 1192487163825246269;
}

void UCustomDiscordGameSubsystem::NativeOnDiscordCoreCreated()
{
	Super::NativeOnDiscordCoreCreated();

	// Automatically set the "Discord Rich Presence" when we get a Discord connection
	UpdateActivity();
}

void UCustomDiscordGameSubsystem::NativeOnDiscordCoreReset()
{
	// Do whatever you need to do to cleanup when the DiscordCore becomes invalid.
	// By the time this is called, Discord is no longer connected and it has already been reset.

	Super::NativeOnDiscordCoreReset();
}

bool UCustomDiscordGameSubsystem::UpdateActivity()
{
	bool bResult {false};

	if (IsDiscordRunning())
	{
		bResult = true;

		discord::Activity Activity {};
		Activity.SetType(discord::ActivityType::Playing);
		Activity.SetApplicationId(ClientId);
		Activity.SetName("Name Here");  // (Note: This value does not seem to be used) TODO HARDCODED
		Activity.SetState("State Here");  // TODO HARDCODED
		Activity.SetDetails("Details Here");  // TODO HARDCODED
		Activity.SetSupportedPlatforms(static_cast<uint32_t>(discord::ActivitySupportedPlatformFlags::Desktop));

		discord::ActivityTimestamps& Timestamps = Activity.GetTimestamps();
		Timestamps.SetStart(FDateTime::UtcNow().ToUnixTimestamp());

		discord::ActivityAssets& Assets = Activity.GetAssets();
		Assets.SetLargeImage("favicon-1024");  // TODO HARDCODED
		Assets.SetLargeText("Large Text");  // TODO HARDCODED
		Assets.SetSmallImage("thumbsup-1024");  // TODO HARDCODED
		Assets.SetSmallText("Small Text");  // TODO HARDCODED

		discord::ActivityParty& Party = Activity.GetParty();
		Party.SetId("1234-5678-9012-3456-7890");  // TODO HARDCODED
		Party.SetPrivacy(discord::ActivityPartyPrivacy::Public);  // TODO HARDCODED
		discord::PartySize& PartySize = Party.GetSize();
		PartySize.SetCurrentSize(1);  // TODO HARDCODED
		PartySize.SetMaxSize(3);  // TODO HARDCODED

		discord::ActivitySecrets& Secrets = Activity.GetSecrets();
		// TODO Secrets info

		DiscordCore().ActivityManager().UpdateActivity(Activity, [this](discord::Result Result)
		{
			const FString RequestDescription (TEXT("Updating Activity"));
			LogDiscordResult(Result, RequestDescription);
		});
	}

	return bResult;
}

#if false
void UCustomDiscordGameSubsystem::ClearActivity()
{
	if (IsDiscordRunning())
	{
		// ClearActivity DOES NOT WORK as of Discord GameSDK 3.2.1
		//
		// @see https://github.com/discord/discord-api-docs/issues/6612
		//
		DiscordCore().ActivityManager().ClearActivity([this](discord::Result Result)
		{
			const FString RequestDescription (TEXT("Clearing Activity"));
			LogDiscordResult(Result, RequestDescription);
		});
	}
}
#endif
