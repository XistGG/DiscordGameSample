
# DiscordGameSample

This is a sample game that integrates UE 5.6 with
[Discord GameSDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)
v3.2.1.

**Don't miss the
[high level overview video](https://youtu.be/HBkw_AYeTJk)
of this repository *(13 minutes)*.**

To see this in action, build and run the game.
As soon as the game (or UEditor) starts, if you have Discord running on your machine,
your Discord status will be updated to show that you're running the game.

## How to Use the `DiscordGame` Plugin in your Game

- Update the SDK to the latest (see [Updating Discord GameSDK](#updating-discord-gamesdk))
- Copy the entire `Plugins/DiscordGame` folder into your project
- Add `DiscordGame` to `PublicDependencyModuleNames` in your `Build.cs`
- Create your own Custom `DiscordGame`-derived Subsystem
  - Use `UCustomDiscordGameSubsystem`
  { [h](./Source/DiscordGameSample/CustomDiscordGameSubsystem.h)
  | [cpp](./Source/DiscordGameSample/CustomDiscordGameSubsystem.cpp)
  } as a starting point *(replace it with your own subsystem)*
- Add whatever gameplay hooks you want, you're now online with Discord!

## Updating Discord GameSDK

- [Setup Part 1: Installing the SDK](#setup-part-1-installing-the-sdk)
- [Setup Part 2: Exporting the SDK](#setup-part-2-exporting-the-sdk)
- [Setup Part 3: Silence UE5 Build Warnings](#setup-part-3-silence-ue5-build-warnings)
- [Setup Part 4: Use UE5-safe Windows.h](#setup-part-4-use-ue5-safe-windowsh)

## Example Discord Status

[![Example Screenshot](./Resources/XistDiscordStatus.png)](./Resources/XistDiscordStatus.png)

# DiscordGame Plugin

The primary point of interest in this repository is the
[DiscordGame](./Plugins/DiscordGame/)
plugin, which contains 2 modules:

## `DiscordGame` Module

- Implements `DiscordGameSubsystem`
  { [h](./Plugins/DiscordGame/Source/DiscordGame/DiscordGameSubsystem.h) |
    [cpp](./Plugins/DiscordGame/Source/DiscordGame/DiscordGameSubsystem.cpp) }
- Dynamically loads `DiscordGameSDK` at runtime
  - Loading managed by [DiscordGame.cpp](./Plugins/DiscordGame/Source/DiscordGame/DiscordGame.cpp)
  - DLL paths must be coordinated with [DiscordGameSDK.Build.cs](./Plugins/DiscordGame/Source/ThirdParty/DiscordGameSDK/DiscordGameSDK.Build.cs)

## `DiscordGameSDK` ThirdParty Module

- ThirdParty Module containing Discord GameSDK distributable DLLs

# Custom Discord Game Subsystem

- Intended to be replaced by your own custom implementation

The underlying `UDiscordGameSubsystem` does all of the actual work of loading the SDK
and managing the connection to the `DiscordCore`,
but it doesn't actually implement any gameplay behavior.

The `UCustomDiscordGameSubsystem`
{ [h](./Source/DiscordGameSample/CustomDiscordGameSubsystem.h)
| [cpp](./Source/DiscordGameSample/CustomDiscordGameSubsystem.cpp)
}
is an example of the kind of custom subsystem you can make for your game,
where you will implement whatever behavior you want.

In this case, it automatically sets the Discord Rich Presence
as soon as we're able to connect to Discord, and it unsets it
when the game ends.

Note that you should **completely replace** `UCustomDiscordGameSubsystem` in your game
with whatever custom subsystem you want.

# Setup Part 1: Installing the SDK

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

# Setup Part 2: Exporting the SDK

After you install the latest SDK, you also need to export all of the classes so you can use them in
other UE5 modules.

You can do this manually, though I created a simple PowerShell script
([ExportGameSDK.ps1](./ExportGameSDK.ps1))
to automate this process.

To see exactly what this script does to the source, see
[this commit](https://github.com/XistGG/DiscordGameSample/commit/f04bf48ea39ae1160a629c4b965dc816cc0f4952)
where I modified the downloaded SDK headers with this script.

To run it for real and modify the Discord GameSDK C++ headers, run it like:

```powershell
ExportGameSDK.ps1
```

You can also execute `ExportGameSDK.ps1` in a debug test mode such that it **will not** make any modifications,
and instead will only show you what it would really do:

```powershell
ExportGameSDK.ps1 -Debug -DryRun
```

### Example output from -Debug -DryRun
[![-Debug -DryRun Terminal Screenshot](./Resources/PSDebugDryRun.png)](./Resources/PSDebugDryRun.png)

*(The actual output is a few screen lengths)*

## Possible Compilation (Linker) Errors

If you fail to export the SDK's C++ classes, when you try to use them in your own UE5 module,
during compilation you will see linker errors such as this:

```text
LNK2019: unresolved external symbol "public: void __cdecl discord::ActivityAssets::SetLargeImage(char const *)" (?SetLargeImage@ActivityAssets@discord@@QEAAXPEBD@Z) referenced in function "public: bool __cdecl UCustomDiscordGameSubsystem::UpdateActivity(void)" (?UpdateActivity@UCustomDiscordGameSubsystem@@QEAA_NXZ)
LNK2019: unresolved external symbol "public: class discord::PartySize & __cdecl discord::ActivityParty::GetSize(void)" (?GetSize@ActivityParty@discord@@QEAAAEAVPartySize@2@XZ) referenced in function "public: bool __cdecl UCustomDiscordGameSubsystem::UpdateActivity(void)" (?UpdateActivity@UCustomDiscordGameSubsystem@@QEAA_NXZ)
LNK2019: unresolved external symbol "public: void __cdecl discord::Activity::SetState(char const *)" (?SetState@Activity@discord@@QEAAXPEBD@Z) referenced in function "public: bool __cdecl UCustomDiscordGameSubsystem::UpdateActivity(void)" (?UpdateActivity@UCustomDiscordGameSubsystem@@QEAA_NXZ)
LNK2019: unresolved external symbol "public: class discord::ActivityManager & __cdecl discord::Core::ActivityManager(void)" (?ActivityManager@Core@discord@@QEAAAEAV02@XZ) referenced in function "public: void __cdecl UCustomDiscordGameSubsystem::ClearActivity(void)" (?ClearActivity@UCustomDiscordGameSubsystem@@QEAAXXZ)
```

To fix these errors, execute [`ExportGameSDK.ps1`](./ExportGameSDK.ps1) and compile again.

# Setup Part 3: Silence UE5 Build Warnings

An optional *(recommended)* setup step is to silence the UE5 Win64 build warnings regarding the use of `strncpy`.

The build will work without this step, but IMO it is annoying to see 20+ warnings every time you build.

The fix is easy. You need to copy/paste 5 lines of code into `types.cpp`, which we previously copied
into the `discord-cpp` directory from the SDK zip extraction.

### Add this to [Plugins/DiscordGame/Source/DiscordGame/discord-cpp/types.cpp](./Plugins/DiscordGame/Source/DiscordGame/discord-cpp/types.cpp)

```c++
//~Begin UE5 change
#ifdef _MSC_VER
#pragma warning(disable:4996)  // Disable warning: Function 'strncpy' is deprecated
#endif
//~End of UE5 change
```

See
[this commit](https://github.com/XistGG/DiscordGameSample/commit/e60c63fc7c15f2657a651cf5c0c5ee3e14129717),
where I applied this setup step to `types.cpp` in this repository.

## What are these Compilation Warnings

The warnings look like this:

```text
0>types.cpp(24): Warning C4996 : 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
```

These warnings are due to Discord GameSDK being ultimately written in C and using old C memory copy methodologies,
which is generally not something you want to do in a modern C++ project.

This isn't **really** a problem as Discord has taken care to ensure that buffer overruns won't actually result from
using these methodologies. You can safely ignore these warnings.

# Setup Part 4: Use UE5-safe Windows.h

UE5 does not like to use `<Windows.h>` without a wrapper.

Discord does use `<Windows.h>` and so it is preferred to modify that include to use the UE5 `WindowsHWrapper.h` instead.

This is a relatively trivial change, but it is required to modify 2 files to effect it.

See
[this commit](https://github.com/XistGG/DiscordGameSample/commit/b6b63589f40bc288bdc8ef88d7e370f902faef66),
where I applied this change.

There are 2 required modifications:

- In header `discord-cpp/ffi.h`
  - DO NOT include `Windows.h`, instead include `Windows/WindowsHWrapper.h`
  - DO include `ole2.h`, which we need.
  - `#define COM_NO_WINDOWS_H 1` for `dxgi.h`
    - This causes `dxgi.h` to NOT include `Windows.h`, which we want.
    - It also causes `dxgi.h` to NOT include `ole2.h`, but we do need `ole2.h`, which is why we now explicitly include it.
- In header `discord-cpp/types.h`
  - DO NOT include `Windows.h` or `dxgi.h`
    - These are both already included by `discord-cpp/ffi.h`
    - Rather than apply the same fix here as we did to `discord-cpp/ffi.h`, we just remove these duplicate includes.
