// Copyright (c) 2024 xist.gg

using UnrealBuildTool;
using System.Collections.Generic;

public class DiscordGameSampleTarget : TargetRules
{
	public DiscordGameSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		ExtraModuleNames.AddRange( new string[] { "DiscordGameSample" } );
	}
}
