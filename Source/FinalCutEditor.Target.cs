using UnrealBuildTool;

public class FinalCutEditorTarget : TargetRules
{
	public FinalCutEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("FinalCut");
	}
}
