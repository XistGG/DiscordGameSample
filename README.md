
# DiscordGameSample

This is a sample game that integrates UE 5.3 with
[Discord GameSDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)
v3.2.1.

## DiscordGame Plugin

The primary point of interest in this repository is the
[DiscordGame](./Plugins/DiscordGame/)
plugin, which contains 2 modules:

### `DiscordGame` Module

- Implements `DiscordGameSubsystem`
  { [h](./Plugins/DiscordGame/Source/DiscordGame/DiscordGameSubsystem.h) |
    [cpp](./Plugins/DiscordGame/Source/DiscordGame/DiscordGameSubsystem.cpp) }
- Dynamically loads `DiscordGameSDK` at runtime (see [DiscordGame.cpp](./Plugins/DiscordGame/Source/DiscordGame/DiscordGame.cpp))
  - DLL paths must be coordinated with [DiscordGameSDK.Build.cs](./Plugins/DiscordGame/Source/ThirdParty/DiscordGameSDK/DiscordGameSDK.Build.cs)

### `DiscordGameSDK` ThirdParty Module

- ThirdParty Module containing Discord GameSDK distributable DLLs

# Setup: Installing the SDK

Current Discord GameSDK version: `v3.2.1`

I downloaded Discord GameSDK from [https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip]()

It unzips into a directory named `discord_game_sdk`, and I then ran the following PowerShell commands
to copy it into the appropriate place into this `DiscordGameSample` UE project:

```powershell
# Copy the binaries from the SDK "lib" dir to "DiscordGameSDK" Source dir
cp -recurse discord_game_sdk/lib/* DiscordGameSample/Plugins/DiscordGame/Source/ThirdParty/DiscordGameSDK/

# Copy the entire "cpp" dir from the SDK to the "discord-cpp" Source dir
cp -recurse discord_game_sdk/cpp DiscordGameSample/Plugins/DiscordGame/Source/DiscordGame/discord-cpp
```

**I recommend that before you run this, you replace the files in the directories above with
the latest versions that you yourself downloaded from Discord.**

If you trust me *(you shouldn't!)* and if there is no newer version of Discord GameSDK,
then you can use the binaries I committed to this repository.

I am not responsible for your computer melting down or any other things that may happen
as a result of you executing binaries that you downloaded from the Internet.
