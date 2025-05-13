// Copyright (c) 2024 xist.gg

using UnrealBuildTool;

public class DiscordGameSample : ModuleRules
{
	public DiscordGameSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"DiscordGame",
			"Engine",
			"InputCore",
		});
	}
}
