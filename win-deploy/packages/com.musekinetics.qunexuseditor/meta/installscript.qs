function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    console.log("Starting createOperations...");

    var targetDir = installer.value("TargetDir");

    // Create the basic operations for the component
    component.createOperations();

    var displayName = component.displayName;
    var description = component.description;

    console.log("DisplayName: " + displayName);
    console.log("Description: " + description);

    if (systemInfo.productType === "windows") {
        var startMenuDir = installer.value("StartMenuDir");
        console.log("StartMenuDir: " + startMenuDir);
        
        var exePath = targetDir + "/" + displayName + "/" + displayName + ".exe";
        var shortcutPath = startMenuDir + "/" + displayName + ".lnk";
        var workingDir = targetDir + "/" + displayName;
        var iconPath = targetDir + "/Content/Icons/quNexusIcon.ico";
        var uninstallShortcutPath = targetDir + "/Uninstall " + displayName + ".lnk";

        console.log("EXE Path: " + exePath);
        console.log("Shortcut Path: " + shortcutPath);
        console.log("Working Directory: " + workingDir);
        console.log("Icon Path: " + iconPath);
        console.log("Uninstall Shortcut Path: " + uninstallShortcutPath);

        // Create a shortcut for the application
        component.addOperation("CreateShortcut", 
                                exePath,
                                shortcutPath,
                                "workingDirectory=" + workingDir,
                                "iconPath=" + iconPath,
                                "description=" + description);

        // Create a shortcut to the maintenance tool for uninstallation
        component.addOperation("CreateShortcut",
                                targetDir + "/maintenancetool.exe",
                                uninstallShortcutPath,
                                "workingDirectory=" + targetDir,
                                "description=Uninstall " + displayName);

        // Install the Visual C++ Redistributable. windeployqt's --compiler-runtime flag only
        // drops vc_redist.x64.exe next to the exe - it does NOT copy loose MSVCP140.dll/
        // VCRUNTIME140.dll, and even if it did, Windows treats those as "Known DLLs" once any
        // VC++ Redistributable has ever been registered on the machine, which forces the OS
        // loader to always use the System32 copies regardless of what's bundled locally. A
        // target machine with an older/mismatched runtime crashes the app immediately
        // (0xc0000005 inside MSVCP140.dll) - confirmed on a clean Windows 11 22H2 VM. Exit
        // codes: 0 = installed, 3010 = installed, reboot required, 1638/5100 = an equal-or-
        // newer version is already present - all four are success, not failure.
        var vcRedistPath = workingDir + "/vc_redist.x64.exe";
        component.addOperation("Execute",
                                "{0,3010,1638,5100}",
                                vcRedistPath,
                                "/install", "/quiet", "/norestart");
    }

    console.log("createOperations completed.");
}
