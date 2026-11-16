using UnrealBuildTool;

// Game module. Besides holding the game's C++ classes it makes the project a C++ project: packaging
// then compiles the game executable with the DLSS / Streamline plugins linked in (a Blueprint-only
// project cannot load them in a packaged build).
public class FinalCut : ModuleRules
{
	public FinalCut(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
		// Lets files include each other by folder, e.g. #include "Characters/FCVoxelPerson.h"
		PublicIncludePaths.Add(ModuleDirectory);
	}
}
