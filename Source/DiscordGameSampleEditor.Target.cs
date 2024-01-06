// Copyright (c) 2024 xist.gg

using UnrealBuildTool;
using System.Collections.Generic;

public class DiscordGameSampleEditorTarget : TargetRules
{
	public DiscordGameSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		ExtraModuleNames.AddRange( new string[] { "DiscordGameSample" } );
	}
}
