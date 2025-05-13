// Copyright (c) 2024 xist.gg

using UnrealBuildTool;
using System.Collections.Generic;

public class DiscordGameSampleTarget : TargetRules
{
	public DiscordGameSampleTarget(TargetInfo Target) : base(Target)
	{
		CppStandard = CppStandardVersion.Cpp20;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "DiscordGameSample" } );
	}
}
