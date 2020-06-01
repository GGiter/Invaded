// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class Invaded : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public Invaded(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "AIModule", "GameplayTasks", "CinematicCamera" });

        PublicDependencyModuleNames.AddRange(new string[] { "RenderCore", "RHI" });

        PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils" });

        PublicDependencyModuleNames.AddRange(new string[] { "AdvancedSessions" });

        PublicDependencyModuleNames.AddRange(new string[] { "HTTP", "Steamworks" });

      //  PublicIncludePaths.AddRange(new string[] { "AdvancedSessions/Classes" });


        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        }

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        LoadUPNP(Target);
    }

    public bool LoadUPNP(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

          //  string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "miniupnpc", "Libraries");

            /*
            test your path with:
            using System; // Console.WriteLine("");
            Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            */

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "miniupnpc"+".lib"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "miniupnpc", "Includes"));
           
        }

        Definitions.Add(string.Format("WITH_MINIUPNPC_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }


}
