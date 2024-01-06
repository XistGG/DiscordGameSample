// Copyright (c) 2024 xist.gg

using UnrealBuildTool;

public class DiscordGame : ModuleRules
{
	public DiscordGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;

		PublicIncludePaths.AddRange(new string[]
		{
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"DiscordGameSDK",
			"Engine",
			"Projects",
		});
	}
}
