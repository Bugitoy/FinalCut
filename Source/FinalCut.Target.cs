using UnrealBuildTool;

public class FinalCutTarget : TargetRules
{
	public FinalCutTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("FinalCut");
	}
}
