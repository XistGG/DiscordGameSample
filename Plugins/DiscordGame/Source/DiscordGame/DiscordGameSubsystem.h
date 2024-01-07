// Copyright (c) 2024 xist.gg

#pragma once

#include "CoreMinimal.h"
#include "discord-cpp/discord.h"
#include "DiscordGame.h"
#include "Subsystems/EngineSubsystem.h"
#include "DiscordGameSubsystem.generated.h"

/**
 * Discord Game Subsystem
 *
 * Base subsystem to manage the Discord GameSDK.
 *
 * This handles loading the Discord GameSDK DLLs and automatically reconnecting
 * to Discord as needed. This doesn't actually DO anything with the GameSDK
 * other than load it and actively manage the connection.
 *
 * You are expected to create your own derivative subsystem to implement any
 * functionality you wish to have in your game. For an example, see
 * `UCustomDiscordGameSubsystem` in the sample project.
 * 
 * @see https://github.com/XistGG/DiscordGameSample/Source/DiscordGameSample/CustomDiscordGameSubsystem.h
 * @see https://github.com/XistGG/DiscordGameSample/Source/DiscordGameSample/CustomDiscordGameSubsystem.cpp
 */
UCLASS()
class DISCORDGAME_API UDiscordGameSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	// Set Class Defaults
	UDiscordGameSubsystem();

	//~USubsystem interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	/**
	 * Have we successfully loaded the Discord GameSDK DLL?
	 *
	 * We'll never be able to manage a Discord connection if we haven't already
	 * loaded the DLL.  If this returns TRUE it DOES NOT MEAN that Discord is
	 * actually installed and running on the current machine.  This only tells
	 * us whether or not the DLL has been loaded.
	 * 
	 * @return TRUE if the Discord GameSDK DLL has been successfully loaded into the Engine, else FALSE
	 */
	bool IsDiscordSDKLoaded() const { return DiscordGameModule && DiscordGameModule->IsDiscordSDKLoaded(); }

	/**
	 * Is Discord currently running on the local machine?
	 *
	 * This tells us whether or not we can attempt to send commands to Discord.
	 * If this returns FALSE you MUST NOT try to obtain a reference to DiscordCore().
	 * If this returns TRUE then you can use DiscordCore().
	 *
	 * @return TRUE if we have a connection to Discord, else FALSE
	 */
	UFUNCTION(BlueprintCallable)
	bool IsDiscordRunning() const { return DiscordCorePtr != nullptr; }

	/**
	 * Get a reference to the Discord Core.
	 *
	 * Before you call this, you MUST check IsDiscordRunning().
	 *
	 * If you try to call this when discord is not running, it will throw an exception,
	 * which will break your game.
	 *
	 * @return DiscordCore reference
	 */
	FORCEINLINE_DEBUGGABLE discord::Core& DiscordCore() const
	{
		checkf(DiscordCorePtr, TEXT("Discord is not running"));
		return *DiscordCorePtr;
	}

protected:
	virtual void NativeOnDiscordCoreCreated();
	virtual void NativeOnDiscordCoreReset();

	virtual void NativeOnDiscordLogMessage(discord::LogLevel Level, const FString& Message) const;

	void LogDiscordResult(discord::Result Result, const FString& RequestDescription) const;

	void SetTickEnabled(bool bWantTicking);

	uint64 ClientId;
	discord::LogLevel MinimumLogLevel;
	float CreateRetryTime;

private:
	bool Tick(float DeltaTime);

	void TryCreateDiscordCore(float DeltaTime);
	void ResetDiscordCore();

	FDiscordGameModule* DiscordGameModule {nullptr};
	discord::Core* DiscordCorePtr {nullptr};

	FTSTicker::FDelegateHandle TickDelegateHandle;
	float RetryWaitRemaining {-1.f};

};
