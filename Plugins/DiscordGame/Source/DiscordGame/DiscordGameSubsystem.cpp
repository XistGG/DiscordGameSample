// Copyright (c) 2024 xist.gg

#include "DiscordGameSubsystem.h"
#include "DiscordGame.h"

void UDiscordGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DiscordGameModule = FDiscordGameModule::Get();
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick));
}

void UDiscordGameSubsystem::Deinitialize()
{
	ClearActivity();

	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	ResetDiscordCore();
	DiscordGameModule = nullptr;
	Super::Deinitialize();
}

bool UDiscordGameSubsystem::UpdateActivity()
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

		DiscordCore->ActivityManager().UpdateActivity(Activity, [this](discord::Result Result)
		{
			const FString RequestDescription (TEXT("Updating Activity"));
			LogDiscordResult(Result, RequestDescription);
		});
	}

	return bResult;
}

void UDiscordGameSubsystem::ClearActivity()
{
	if (IsDiscordRunning())
	{
		DiscordCore->ActivityManager().ClearActivity([this](discord::Result Result)
		{
			const FString RequestDescription (TEXT("Clearing Activity"));
			LogDiscordResult(Result, RequestDescription);
		});
	}
}

void UDiscordGameSubsystem::NativeOnDiscordCoreCreated()
{
	check(DiscordCore);

	DiscordCore->SetLogHook(MinimumLogLevel, [this](discord::LogLevel Level, const char* RawMessage)
	{
		const FString Message (RawMessage);
		NativeOnDiscordLogMessage(Level, Message);
	});

	UpdateActivity();
}

void UDiscordGameSubsystem::NativeOnDiscordLogMessage(discord::LogLevel Level, const FString& Message) const
{
	switch (Level)
	{
	case discord::LogLevel::Debug:
		UE_LOG(LogDiscord, Verbose, TEXT("%s"), *Message);
		break;
	case discord::LogLevel::Info:
		UE_LOG(LogDiscord, Log, TEXT("%s"), *Message);
		break;
	case discord::LogLevel::Warn:
		UE_LOG(LogDiscord, Warning, TEXT("%s"), *Message);
		break;
	case discord::LogLevel::Error:
	default:
		UE_LOG(LogDiscord, Error, TEXT("%s"), *Message);
		break;
	}
}

void UDiscordGameSubsystem::LogDiscordResult(discord::Result Result, const FString& RequestDescription) const
{
	switch (Result)
	{
	case discord::Result::Ok:
		UE_LOG(LogDiscord, Log, TEXT("Success %s"), *RequestDescription);
		break;
	default:
		UE_LOG(LogDiscord, Error, TEXT("Error(%i) %s"), Result, *RequestDescription);
		break;
	}
}

bool UDiscordGameSubsystem::Tick(float DeltaTime)
{
	if (IsDiscordRunning())
	{
		const discord::Result Result = DiscordCore->RunCallbacks();
		switch (Result)
		{
		case discord::Result::Ok:
			// The expected result; Discord is functioning normally
			break;

		case discord::Result::NotRunning:
			// Discord is no longer running, it is no longer usable and will never be again
			// unless/until we successfully initialize a new DiscordCore. 
			UE_LOG(LogDiscord, Error, TEXT("Error(%i) Running Callbacks; Discord is no longer running"), Result);
			ResetDiscordCore();
			break;

		default:
			// Unknown error, hopefully Discord can recover from this...
			// For now we just spam the log every tick with errors so you know what is happening.
			//
			// You will need to experiment with this to determine what errors are recoverable
			// and what errors are not, and manage them better than this:
			UE_LOG(LogDiscord, Error, TEXT("Error(%i) Running Callbacks"), Result);
			break;
		}
	}
	else if (IsDiscordSDKLoaded())
	{
		TryCreateDiscordCore(DeltaTime);
	}

	return true;
}

void UDiscordGameSubsystem::TryCreateDiscordCore(float DeltaTime)
{
	RetryWaitRemaining -= DeltaTime;

	if (RetryWaitRemaining <= 0.f)
	{
		switch (const discord::Result Result = discord::Core::Create(ClientId, DiscordCreateFlags_NoRequireDiscord, &DiscordCore))
		{
		case discord::Result::Ok:
			UE_LOG(LogDiscord, Log, TEXT("Created Discord Core"));
			NativeOnDiscordCoreCreated();
			break;

		case discord::Result::InternalError:
			UE_LOG(LogDiscord, Error, TEXT("Error(%i) Creating Discord Core; Discord not running?"), Result);
			break;

		default:
			UE_LOG(LogDiscord, Error, TEXT("Error(%i) Creating Discord Core"), Result);
			break;
		}

		// Don't try to create every single tick; wait some time
		RetryWaitRemaining = CreateRetryTime;
	}
}

void UDiscordGameSubsystem::ResetDiscordCore()
{
	if (DiscordCore)
	{
		// Discord GameSDK doesn't provide any way to free memory !?
		// It allocates memory with new(), so here we will delete.
		delete DiscordCore;
		DiscordCore = nullptr;
	}

	// Ensure that next tick we'll immediately try reconnecting (if still ticking)
	RetryWaitRemaining = -1;
}
