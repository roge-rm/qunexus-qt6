param(
    [switch]$DisableSigning
)

$ErrorActionPreference = 'Stop'

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message"
}

function Get-VersionFromProFile {
    param([string]$ProFilePath)

    $versionLine = Select-String -Path $ProFilePath -Pattern '^VERSION\s*=\s*(.+)$' | Select-Object -First 1
    if (-not $versionLine) {
        throw "Could not find VERSION in $ProFilePath"
    }

    return $versionLine.Matches[0].Groups[1].Value.Trim()
}

function Set-XmlNodeValue {
    param(
        [string]$FilePath,
        [string]$XPath,
        [string]$Value
    )

    if (-not (Test-Path -LiteralPath $FilePath)) {
        throw "Required XML file not found: $FilePath"
    }

    $xml = New-Object System.Xml.XmlDocument
    $xml.PreserveWhitespace = $true
    $xml.Load($FilePath)

    $node = $xml.SelectSingleNode($XPath)
    if (-not $node) {
        throw "Could not find XML node '$XPath' in $FilePath"
    }

    $node.InnerText = $Value
    $xml.Save($FilePath)
}

function Update-InstallerMetadata {
    param(
        [string]$InstallerVersion,
        [string]$ConfigFile,
        [string[]]$PackageFiles
    )

    Write-Step 'Updating installer metadata versions'
    Set-XmlNodeValue -FilePath $ConfigFile -XPath '/Installer/Version' -Value $InstallerVersion

    foreach ($packageFile in $PackageFiles) {
        Set-XmlNodeValue -FilePath $packageFile -XPath '/Package/Version' -Value $InstallerVersion
    }
}

function Ensure-Directory {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        New-Item -ItemType Directory -Path $Path -Force | Out-Null
    }
}

function Copy-RequiredFile {
    param(
        [string]$Source,
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source)) {
        throw "Required file not found: $Source"
    }

    Copy-Item -LiteralPath $Source -Destination $Destination -Force
}

function Invoke-External {
    param(
        [string]$FilePath,
        [string[]]$ArgumentList,
        [string]$WorkingDirectory
    )

    $displayArgs = ($ArgumentList | ForEach-Object {
        if ($_ -match '\s') { '"' + $_ + '"' } else { $_ }
    }) -join ' '
    Write-Host "$FilePath $displayArgs"

    Push-Location -LiteralPath $WorkingDirectory
    try {
        & $FilePath @ArgumentList
        if ($LASTEXITCODE -ne 0) {
            throw "Command failed with exit code ${LASTEXITCODE}: $FilePath"
        }
    }
    finally {
        Pop-Location
    }
}

function Invoke-Signing {
    param(
        [string]$SignTool,
        [string]$TimestampUrl,
        [string]$FileToSign,
        [string]$WorkingDirectory
    )

    if ($DisableSigning) {
        Write-Host "Skipping signing for $FileToSign because -DisableSigning was supplied."
        return
    }

    # /fd certHash is not a valid digest algorithm (only sha1/sha256 are) - this
    # matches the proven-working manual command in sendsysex's RELEASING.md and
    # the fix applied to 12 Step/SoftStep's copies of this same script/template.
    Invoke-External -FilePath $SignTool -ArgumentList @(
        'sign', '/fd', 'sha256', '/a',
        '/tr', $TimestampUrl,
        '/td', 'SHA256',
        $FileToSign
    ) -WorkingDirectory $WorkingDirectory
}

function Copy-CurrentVcRedist {
    param([string]$PackageDir)

    # windeployqt's --compiler-runtime bundles whatever vc_redist.x64.exe happens to be cached in
    # this Qt install's own tooling directory, which can be years stale and target an OLDER VC++
    # ABI than the actual MSVC toolset the app was compiled with (confirmed on 2026-08-03: the
    # cached copy was "2015-2019 Redistributable (x64) - 14.27.29016" while this build used MSVC
    # toolset 14.44 - installing the stale one on a clean machine no-ops against a same-or-newer
    # preinstalled runtime without ever satisfying what the app actually needs, so the app crashes
    # on launch, 0xc0000005 inside MSVCP140.dll - see SOP §4.10). Overwrite it with the redistributable
    # that ships alongside the actual installed VS2022 toolset instead of trusting windeployqt's cache.
    Write-Step 'Copying current VC++ Redistributable (overrides windeployqt''s possibly-stale cached copy)'
    $vsRedistRoots = @(
        'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Redist\MSVC',
        'C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Redist\MSVC',
        'C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC',
        'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC',
        'C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\VC\Redist\MSVC',
        'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Redist\MSVC',
        'C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise\VC\Redist\MSVC',
        'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Redist\MSVC'
    )

    $currentVcRedist = $null
    foreach ($root in $vsRedistRoots) {
        if (-not (Test-Path -LiteralPath $root)) { continue }
        $candidate = Get-ChildItem -LiteralPath $root -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '^\d+\.\d+\.\d+$' -and (Test-Path -LiteralPath (Join-Path $_.FullName 'vc_redist.x64.exe')) } |
            Sort-Object { [version]$_.Name } -Descending |
            Select-Object -First 1
        if ($candidate) {
            $currentVcRedist = Join-Path $candidate.FullName 'vc_redist.x64.exe'
            break
        }
    }

    if ($currentVcRedist) {
        Write-Host "Using $currentVcRedist"
        Copy-RequiredFile -Source $currentVcRedist -Destination $PackageDir
    } else {
        Write-Warning "Could not find a current vc_redist.x64.exe under any known VS2022 install root - keeping windeployqt's bundled copy, which may be stale (see SOP §4.10). Installed app may crash on a clean machine."
    }
}

function Copy-EditorPayload {
    param(
        [string]$PackageDir,
        [string]$ReleaseExe,
        [string]$DisplayName,
        [string]$QtDeployTool,
        [string]$SslCrypto,
        [string]$SslTls,
        [string]$SignTool
    )

    Ensure-Directory -Path $PackageDir

    $mainDest = Join-Path $PackageDir "$DisplayName.exe"

    Write-Step "Staging $DisplayName"
    Copy-RequiredFile -Source $ReleaseExe -Destination $mainDest

    Invoke-Signing -SignTool $SignTool -TimestampUrl 'http://timestamp.digicert.com' -FileToSign $mainDest -WorkingDirectory $PackageDir

    Write-Step "Running windeployqt for $DisplayName"
    Invoke-External -FilePath $QtDeployTool -ArgumentList @(
        '--compiler-runtime',
        '--dir', $PackageDir,
        $mainDest
    ) -WorkingDirectory $PackageDir

    Copy-CurrentVcRedist -PackageDir $PackageDir

    Write-Step "Copying SSL runtime for $DisplayName"
    Copy-RequiredFile -Source $SslCrypto -Destination $PackageDir
    Copy-RequiredFile -Source $SslTls -Destination $PackageDir
}

$workspaceRoot = Split-Path -Parent $PSScriptRoot
$editorRoot = Join-Path $workspaceRoot 'qt-qunexus'
$winDeployRoot = Join-Path $workspaceRoot 'win-deploy'

$editorPro = Join-Path $editorRoot 'QuNexus.pro'

$installerVersion = Get-VersionFromProFile -ProFilePath $editorPro
$installerName = "QuNexus Editor v$installerVersion Windows Installer"
$installerFile = "$installerName.exe"

$qtDeployTool = 'C:\Qt6\6.3.2\msvc2019_64\bin\windeployqt.exe'
$binaryCreator = 'C:\Qt6\QtIFW-4.6.0\bin\binarycreator.exe'
$signTool = 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe'

$sslCrypto = Join-Path $editorRoot 'inc\KMI_MIDI_Device_Manager\ssl\libcrypto-1_1-x64.dll'
$sslTls = Join-Path $editorRoot 'inc\KMI_MIDI_Device_Manager\ssl\libssl-1_1-x64.dll'

$editorReleaseDir = Join-Path $editorRoot 'build-QuNexus-Desktop_Qt_6_3_2_MSVC2019_64bit-Release\release'

$packageDir = Join-Path $winDeployRoot 'packages\com.musekinetics.qunexuseditor\data\QuNexus Editor'
$contentDir = Join-Path $winDeployRoot 'packages\com.musekinetics.qunexuseditor\data\Content'

$editorReleaseExe = Join-Path $editorReleaseDir 'QuNexus Editor.exe'

$contentSource = Join-Path $workspaceRoot 'Content'
$changelogSource = Join-Path $workspaceRoot 'CHANGELOG.md'
$configFile = Join-Path $winDeployRoot 'config\config.xml'
$packagesDir = Join-Path $winDeployRoot 'packages'
$installerOutputPath = Join-Path $winDeployRoot $installerFile
$packageMetaFile = Join-Path $winDeployRoot 'packages\com.musekinetics.qunexuseditor\meta\package.xml'

Write-Host "Installer version: $installerVersion"
if ($DisableSigning) {
    Write-Host 'Signing is disabled for this run.'
}

Update-InstallerMetadata -InstallerVersion $installerVersion -ConfigFile $configFile -PackageFiles @(
    $packageMetaFile
)

Copy-EditorPayload -PackageDir $packageDir -ReleaseExe $editorReleaseExe -DisplayName 'QuNexus Editor' -QtDeployTool $qtDeployTool -SslCrypto $sslCrypto -SslTls $sslTls -SignTool $signTool

Write-Step 'Refreshing installer content package'
if (Test-Path -LiteralPath $contentDir) {
    Remove-Item -LiteralPath $contentDir -Recurse -Force
}
Ensure-Directory -Path $contentDir
# -LiteralPath disables wildcard expansion, so a trailing '\*' is treated as a literal
# (nonexistent) path component - Copy-Item then silently no-ops instead of throwing,
# even under $ErrorActionPreference = 'Stop', because no error is actually raised.
# Confirmed this left every installer built this session with an empty Content package
# (just the CHANGELOG.md added separately below) - missing DAW Support/Manual/SysEx AND
# the Icons/ folder installscript.qs's shortcut iconPath points at, which is why the
# Start Menu shortcut had no icon. -Path supports wildcards; use that instead.
Copy-Item -Path (Join-Path $contentSource '*') -Destination $contentDir -Recurse -Force
Copy-RequiredFile -Source $changelogSource -Destination $contentDir

Write-Step 'Creating installer'
if (Test-Path -LiteralPath $installerOutputPath) {
    Remove-Item -LiteralPath $installerOutputPath -Force
}
Invoke-External -FilePath $binaryCreator -ArgumentList @(
    '--verbose',
    '--offline-only',
    '-c', $configFile,
    '-p', $packagesDir,
    $installerName
) -WorkingDirectory $winDeployRoot

Write-Step 'Signing installer'
Invoke-Signing -SignTool $signTool -TimestampUrl 'http://timestamp.globalsign.com/tsa/advanced' -FileToSign $installerOutputPath -WorkingDirectory $winDeployRoot

Write-Host "`nInstaller created at $installerOutputPath"
