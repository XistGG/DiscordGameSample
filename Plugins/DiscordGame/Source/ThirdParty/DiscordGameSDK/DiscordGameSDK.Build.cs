// Copyright (c) 2024 xist.gg

using System.IO;
using UnrealBuildTool;

public class DiscordGameSDK : ModuleRules
{
	public DiscordGameSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x86_64", "discord_game_sdk.dll.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("discord_game_sdk.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add("$(TargetOutputDir)/discord_game_sdk.dll", "$(PluginDir)/Source/ThirdParty/DiscordGameSDK/x86_64/discord_game_sdk.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
	        string ArchDir = (Target.Architecture.ToString() == "x86_64") ? "x86_64" : "aarch64";
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, ArchDir, "discord_game_sdk.dylib"));
            RuntimeDependencies.Add("$(TargetOutputDir)/discord_game_sdk.dylib", Path.Combine("$(PluginDir)/Source/ThirdParty/DiscordGameSDK", ArchDir, "discord_game_sdk.dylib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			string soPath = "$(PluginDir)/Source/ThirdParty/DiscordGameSDK/x86_64/discord_game_sdk.so";
			PublicAdditionalLibraries.Add(soPath);
			PublicDelayLoadDLLs.Add(soPath);
			RuntimeDependencies.Add("$(TargetOutputDir)/discord_game_sdk.so", soPath);
		}
	}
}
