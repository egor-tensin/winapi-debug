param(
    [string] $BuildDir = 'C:\Projects\build',
    [string] $ProjectDir = $null,
    [string] $Platform = $null,
    [string] $Configuration = $null,
    [string] $BoostDir = $null,
    [string] $BoostLibraryDir = $null
)

$ErrorActionPreference = "Stop";

function Invoke-Exe {
    param(
        [ScriptBlock] $Cmd,
        [int[]] $AllowedExitCodes = @(0)
    )

    $backupErrorActionPreference = $script:ErrorActionPreference
    $script:ErrorActionPreference = 'Continue'

    try {
        & $Cmd
        if ($AllowedExitCodes -notcontains $LastExitCode) {
            throw "External command failed with exit code ${LastExitCode}: $Cmd"
        }
    } finally {
        $script:ErrorActionPreference = $backupErrorActionPreference
    }
}

function Test-AppVeyor {
    return Test-Path env:APPVEYOR
}

function Format-AppVeyorBoostDir {
    return "boost_" + $env:appveyor_boost_version.replace('.', '_')
}

function Get-AppVeyorBoostDir {
    return "C:\Libraries\$(Format-AppVeyorBoostDir)"
}

function Get-AppVeyorBoostLibraryDir {
    param(
        [string] $Platform = $env:PLATFORM
    )

    $BoostDir = Get-AppVeyorBoostDir

    if ($Platform -eq 'x64') {
        return "$BoostDir\lib64-msvc-14.1"
    } else {
        return "$BoostDir\lib32-msvc-14.1"
    }
}

function Build-Project {
    param(
        [Parameter(Mandatory=$true)]
        [string] $ProjectDir,
        [Parameter(Mandatory=$true)]
        [string] $BuildDir,
        [Parameter(Mandatory=$true)]
        [string] $Platform,
        [Parameter(Mandatory=$true)]
        [string] $Configuration,
        [Parameter(Mandatory=$true)]
        [string] $BoostDir,
        [string] $BoostLibraryDir = $null
    )

    if (-not $BoostLibraryDir) {
        $BoostLibraryDir = "$BoostDir\stage\lib"
    }

    mkdir $BuildDir
    cd $BuildDir

    Invoke-Exe { cmake -Wno-dev                 `
        -G "Visual Studio 14 2015" -A $Platform `
        -D "BOOST_ROOT=$BoostDir"               `
        -D "BOOST_LIBRARYDIR=$BoostLibraryDir"  `
        -D Boost_USE_STATIC_LIBS=ON             `
        -D Boost_USE_STATIC_RUNTIME=ON          `
        $ProjectDir
    }
    
    Invoke-Exe { cmake --build . --config "$Configuration" -- /m }
}

if (Test-AppVeyor) {
    $cwd = pwd
    $ProjectDir = $env:APPVEYOR_BUILD_FOLDER
    $BuildDir = 'C:\Projects\build'
    $Platform = $env:PLATFORM
    $Configuration = $env:CONFIGURATION
    $BoostDir = Get-AppVeyorBoostDir
    $BoostLibraryDir = Get-AppVeyorBoostLibraryDir -Platform $Platform
}

Build-Project                         `
    -ProjectDir $ProjectDir           `
    -BuildDir $BuildDir               `
    -Platform $Platform               `
    -Configuration $Configuration     `
    -BoostDir $BoostDir               `
    -BoostLibraryDir $BoostLibraryDir

if (Test-AppVeyor) {
    cd $cwd
}
