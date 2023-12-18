﻿using Sharpmake;
using System.IO;
using System.Xml.Linq;
using static CommonTarget;

public abstract class BaseProject : Project
{
    public BaseProject()
        : base(typeof(CommonTarget))
    {
        Name = "BaseProject";
        SourceRootPath = @"Source";
        IsFileNameToLower = false;
        IsTargetFileNameToLower = false;
        AddTargets(CommonTarget.GetDefaultTargets());
    }

    public static class ConfigurePriorities
    {
        public const int All = -75;
        public const int Platform = -50;
        public const int Optimization = -25;
        /*     SHARPMAKE DEFAULT IS 0     */
        public const int Blobbing = 10;
        public const int BuildSystem = 30;
    }

    [ConfigurePriority(ConfigurePriorities.All)]
    [Configure]
    public virtual void ConfigureAll(Project.Configuration conf, CommonTarget target)
    {
        conf.ProjectPath = Path.Combine("[project.SharpmakeCsPath]", ".tmp/project");

        // It seems like you use <> includes a lot, so this is needed per project
        conf.IncludePaths.Add(Path.Combine("[project.SharpmakeCsPath]", "Src"));
        conf.IncludePaths.Add(Globals.IncludeDir);

        //conf.Options.Add(Sharpmake.Project.Configuration.LocalDebuggerWorkingDirectory.Vc.Compiler.Exceptions.Enable);
        conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
        //conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Globals.RootDir;
        conf.VcxprojUserFile.OverwriteExistingFile = true;
        conf.ProjectFileName = @"[project.Name]_[target.Platform]";
        //public static string LibDir = RootDir.."_Build/".."%{cfg.buildcfg}/".."%{cfg.platform}"

        conf.TargetPath = Path.Combine(Globals.RootDir, "_Build/[target.Name]/[target.Platform]/");
        conf.LibraryPaths.Add("[project.SharpmakeCsPath]/.build/[target.Name]/");

        // RenderDoc DLL
        // TODO: Is this a chocolatey package? cross platform?
        // Shitty detection atm, but it works without this?
        if (File.Exists("C:/Program Files/RenderDoc/renderdoc.dll"))
        {
            conf.Defines.Add("DEFINE_ME_RENDERDOC");
            conf.IncludePaths.Add("C:/Program Files/RenderDoc/");
            // If we have RenderDoc installed, it better be the same version 😤
            var copyDirBuildStep = new Configuration.BuildStepCopy(
                $@"[project.SharpmakeCsPath]/ThirdParty/RenderDoc",
                Globals.RootDir + "/.build/[target.Name]");

            copyDirBuildStep.IsFileCopy = false;
            copyDirBuildStep.CopyPattern = "*.dll";
            conf.EventPostBuildExe.Add(copyDirBuildStep);
        }

        //conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);

        conf.Defines.Add("NOMINMAX");

        if (Directory.Exists(Globals.FMOD_Win64_Dir) || Directory.Exists(Globals.FMOD_UWP_Dir))
        {
            conf.Defines.Add("_DISABLE_EXTENDED_ALIGNED_STORAGE");
        }

        if (target.Optimization == Optimization.Debug)
        {
        }
        else
        {

        }
    }

    #region Platfoms

    [ConfigurePriority(ConfigurePriorities.Platform)]
    [Configure(SubPlatformType.x64)]
    public virtual void ConfigureWin64(Configuration conf, CommonTarget target)
    {
        conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP17);
        conf.Options.Add(Options.Vc.Compiler.RTTI.Enable);
        conf.Options.Add(Options.Vc.General.CharacterSet.Unicode);
        conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);

        conf.Defines.Add("DEFINE_ME_PLATFORM_WIN64");

        conf.Options.Add(
            new Options.Vc.Compiler.DisableSpecificWarnings(
                "4201",
                "4100"
                )
        );

        if (Directory.Exists(Globals.FMOD_Win64_Dir))
        {
            conf.Defines.Add("DEFINE_ME_FMOD");
        }
        if (Directory.Exists(Globals.MONO_Win64_Dir))
        {
            conf.Defines.Add("DEFINE_ME_MONO");
        }
    }

    #endregion

    #region Optimizations

    [ConfigurePriority(ConfigurePriorities.Optimization)]
    [Configure(Optimization.Debug)]
    public virtual void ConfigureDebug(Configuration conf, CommonTarget target)
    {
        conf.DefaultOption = Options.DefaultTarget.Debug;

        conf.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
        conf.Options.Add(Options.Vc.Compiler.Inline.Disable);
    }

    [ConfigurePriority(ConfigurePriorities.Optimization)]
    [Configure(Optimization.Release)]
    public virtual void ConfigureRelease(Configuration conf, CommonTarget target)
    {
        conf.DefaultOption = Options.DefaultTarget.Release;
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
        conf.Options.Add(Options.Vc.Compiler.Inline.OnlyInline);
    }


    [ConfigurePriority(ConfigurePriorities.Optimization)]
    [Configure(Optimization.Retail)]
    public virtual void ConfigureRetail(Configuration conf, CommonTarget target)
    {
        conf.DefaultOption = Options.DefaultTarget.Release;

        conf.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);

        // Full inlining
        conf.Options.Add(Options.Vc.Compiler.Inline.AnySuitable);
        conf.Options.Add(Options.Vc.Compiler.Optimization.MaximizeSpeed);

        conf.Options.Add(Options.Vc.Compiler.Exceptions.EnableWithSEH);
        conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions2);
    }

    #endregion
}