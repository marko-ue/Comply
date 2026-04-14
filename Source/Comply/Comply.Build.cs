// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Comply : ModuleRules
{
	public Comply(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Comply",
			"Comply/Variant_Platforming",
			"Comply/Variant_Platforming/Animation",
			"Comply/Variant_Combat",
			"Comply/Variant_Combat/AI",
			"Comply/Variant_Combat/Animation",
			"Comply/Variant_Combat/Gameplay",
			"Comply/Variant_Combat/Interfaces",
			"Comply/Variant_Combat/UI",
			"Comply/Variant_SideScrolling",
			"Comply/Variant_SideScrolling/AI",
			"Comply/Variant_SideScrolling/Gameplay",
			"Comply/Variant_SideScrolling/Interfaces",
			"Comply/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
