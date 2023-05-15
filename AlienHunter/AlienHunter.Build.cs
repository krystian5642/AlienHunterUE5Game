// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AlienHunter : ModuleRules
{
    public AlienHunter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem",
            "UMG",
            "Slate",
            "SlateCore",
            "Niagara",
            "PhysicsCore",
            "UMG"
        });
    }
}
