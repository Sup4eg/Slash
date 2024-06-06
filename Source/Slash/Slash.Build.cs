// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Slash : ModuleRules
{
	public Slash(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "HairStrandsCore", "Niagara", "GeometryCollectionEngine", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Slash/Public/Items/Weapons",
			"Slash/Public/Items/Pawns",
			"Slash/Public/Characters",
			"Slash/Public/Enemy",
			"Slash/Public/Interfaces",
			"Slash/Public/Breakable",
			"Slash/Public/Pawns",
			"Slash/Public/Components",
			"Slash/Public/HUD"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
