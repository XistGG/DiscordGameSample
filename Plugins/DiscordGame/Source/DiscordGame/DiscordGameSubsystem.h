// Copyright (c) 2024 xist.gg

#pragma once

#include "CoreMinimal.h"
#include "discord-cpp/discord.h"
#include "DiscordGame.h"
#include "Subsystems/EngineSubsystem.h"
#include "DiscordGameSubsystem.generated.h"

/**
 * Discord Game Subsystem
 */
UCLASS()
class DISCORDGAME_API UDiscordGameSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	bool IsDiscordSDKLoaded() const { return DiscordGameModule && DiscordGameModule->IsDiscordSDKLoaded(); }
	bool IsDiscordRunning() const { return DiscordCore != nullptr; }

	bool UpdateActivity();
	void ClearActivity();

protected:
	void NativeOnDiscordCoreCreated();
	void NativeOnDiscordLogMessage(discord::LogLevel Level, const FString& Message) const;

	void LogDiscordResult(discord::Result Result, const FString& RequestDescription) const;

	const uint64 ClientId {1192487163825246269};
	const discord::LogLevel MinimumLogLevel {discord::LogLevel::Debug};
	const float CreateRetryTime {0.5f};

private:
	bool Tick(float DeltaTime);

	void TryCreateDiscordCore(float DeltaTime);
	void ResetDiscordCore();

	FDiscordGameModule* DiscordGameModule;
	FTSTicker::FDelegateHandle TickDelegateHandle;

	float RetryWaitRemaining {-1.f};
	discord::Core* DiscordCore {nullptr};

};
