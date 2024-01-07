// Copyright (c) 2024 xist.gg

#include "DiscordGameSubsystem.h"
#include "DiscordGame.h"

UDiscordGameSubsystem::UDiscordGameSubsystem()
{
	// You need to override these settings in your derived subsystem.
	// These should be configured for your own game:
	ClientId = 1192487163825246269;
	MinimumLogLevel = discord::LogLevel::Debug;
	CreateRetryTime = 0.5f;
}

bool UDiscordGameSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), OUT ChildClasses, false);

	UE_LOG(LogDiscord, Log, TEXT("Found %i derived classes when attemping to create DiscordGameSubsystem (%s)"), ChildClasses.Num(), *GetClass()->GetName());

	// Only create an instance if there is no override implementation defined elsewhere
	return ChildClasses.Num() == 0;
}

void UDiscordGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DiscordGameModule = FDiscordGameModule::Get();
	SetTickEnabled(true);
}

void UDiscordGameSubsystem::Deinitialize()
{
	SetTickEnabled(false);
	ResetDiscordCore();
	DiscordGameModule = nullptr;
	Super::Deinitialize();
}

void UDiscordGameSubsystem::NativeOnDiscordCoreCreated()
{
	check(DiscordCorePtr);

	DiscordCorePtr->SetLogHook(MinimumLogLevel, [this](discord::LogLevel Level, const char* RawMessage)
	{
		const FString Message (UTF8_TO_TCHAR(RawMessage));
		NativeOnDiscordLogMessage(Level, Message);
	});
}

void UDiscordGameSubsystem::NativeOnDiscordCoreReset()
{
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

void UDiscordGameSubsystem::SetTickEnabled(bool bWantTicking)
{
	if (bWantTicking && !TickDelegateHandle.IsValid())
	{
		// Want to enable ticking and it is not currently enabled
		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick));
	}
	else if (!bWantTicking && TickDelegateHandle.IsValid())
	{
		// Want to disable ticking and it is currently enabled
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
		TickDelegateHandle.Reset();
	}
}

bool UDiscordGameSubsystem::Tick(float DeltaTime)
{
	if (IsDiscordRunning())
	{
		const discord::Result Result = DiscordCorePtr->RunCallbacks();
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
		switch (const discord::Result Result = discord::Core::Create(ClientId, DiscordCreateFlags_NoRequireDiscord, &DiscordCorePtr))
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
	if (DiscordCorePtr)
	{
		// Discord GameSDK doesn't provide any way to free memory !?
		// It allocates memory with new(), so here we will delete.
		delete DiscordCorePtr;
		DiscordCorePtr = nullptr;

		// Allow child classes the opportunity to react to this event
		NativeOnDiscordCoreReset();
	}

	// Ensure that next tick we'll immediately try reconnecting (if still ticking)
	RetryWaitRemaining = -1;
}
