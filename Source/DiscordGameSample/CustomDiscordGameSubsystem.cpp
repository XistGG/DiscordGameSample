﻿// Copyright (c) 2024 xist.gg

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
	// Clear out any "Discord Rich Presence" (if any) when we're disconnecting from Discord
	ClearActivity();

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
		Activity.SetName("NameHere");  // TODO HARDCODED
		Activity.SetState("StateHere");  // TODO HARDCODED
		Activity.SetDetails("DetailsHere");  // TODO HARDCODED
		Activity.SetSupportedPlatforms(static_cast<uint32_t>(discord::ActivitySupportedPlatformFlags::Desktop));

		discord::ActivityTimestamps& Timestamps = Activity.GetTimestamps();
		Timestamps.SetStart(FDateTime::UtcNow().ToUnixTimestamp());

		discord::ActivityAssets& Assets = Activity.GetAssets();
		Assets.SetLargeImage("favicon-1024");  // TODO HARDCODED
		Assets.SetLargeText("LargeText");  // TODO HARDCODED
		Assets.SetSmallImage("favicon-1024");  // TODO HARDCODED
		Assets.SetSmallText("LargeText");  // TODO HARDCODED

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

void UCustomDiscordGameSubsystem::ClearActivity()
{
	if (IsDiscordRunning())
	{
		DiscordCore().ActivityManager().ClearActivity([this](discord::Result Result)
		{
			const FString RequestDescription (TEXT("Clearing Activity"));
			LogDiscordResult(Result, RequestDescription);
		});
	}
}