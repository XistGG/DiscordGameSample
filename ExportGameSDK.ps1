#
# ExportGameSDK.ps1
#
#   This PowerShell script will modify the Discord GameSDK cpp source
#   such that everything is publicly exported.
#
#   This will allow the GameSDK classes to be used by other UE5 modules
#   in addition to the DiscordGame that actually distributes the code.
#
#   If you update the GameSDK cpp source, YOU MUST RUN THIS (or something
#   like it) to export the newly updated source.
#
#   Failure to run this will result in many linker errors such as these:
#
#   LNK2019: unresolved external symbol "public: class discord::ActivityManager & __cdecl discord::Core::ActivityManager(void)" (?ActivityManager@Core@discord@@QEAAAEAV02@XZ) referenced in function "public: void __cdecl UCustomDiscordGameSubsystem::ClearActivity(void)" (?ClearActivity@UCustomDiscordGameSubsystem@@QEAAXXZ)
#   LNK2019: unresolved external symbol "public: void __cdecl discord::ActivityAssets::SetLargeImage(char const *)" (?SetLargeImage@ActivityAssets@discord@@QEAAXPEBD@Z) referenced in function "public: bool __cdecl UCustomDiscordGameSubsystem::UpdateActivity(void)" (?UpdateActivity@UCustomDiscordGameSubsystem@@QEAA_NXZ)
#   LNK2019: unresolved external symbol "public: void __cdecl discord::ActivityTimestamps::SetStart(__int64)" (?SetStart@ActivityTimestamps@discord@@QEAAX_J@Z) referenced in function "public: bool __cdecl UCustomDiscordGameSubsystem::UpdateActivity(void)" (?UpdateActivity@UCustomDiscordGameSubsystem@@QEAA_NXZ)
#
#   For this script to work, it MUST be in the root project folder,
#   the same folder in which DiscordGameSample.uproject exists.
#

[CmdletBinding()]
param(
    [switch]$DryRun
)

################################################################################
###  Init
################################################################################

# The export string to add
$ApiExport = "DISCORDGAME_API"

# The absolute path to the "discord-cpp" source dir
$CppDir = Join-Path -Path $PSScriptRoot -ChildPath "Plugins/DiscordGame/Source/DiscordGame/discord-cpp"

Write-Debug "Processing directory: $CppDir"

# List of all *.h files in the "discord-cpp" source dir.
# We use this convention (-Recurse -Include "*.h") to ensure we get ABSOLUTE filenames here
$HeaderFiles = Get-ChildItem -Path $CppDir -File -Recurse -Include "*.h"


################################################################################
###  Main
################################################################################

# Create a temp file
$TempFile = [System.IO.Path]::GetTempFileName()
if (!$TempFile) { throw "Cannot create temp file" }

$NumErrors = 0

foreach ($File in $HeaderFiles)
{
    # Read the content from the source file
    $OriginalContent = Get-Content -Path $File

    if (!$OriginalContent)
    {
        Write-Warning "Failed to read content from file: $File"
        $NumErrors++
        continue
    }

    # Inject the API Export macros into each class declaration
    # (DO NOT add it more than once!)
    $NewContent = $OriginalContent -replace "^(\s*class\s+)((?!$ApiExport)\S+.*)","`$1$ApiExport `$2"

    # Write the new content to the temp file
    Set-Content -Path $TempFile -Value $NewContent

    if ($DryRun)
    {
        # In a -DryRun, do not actually overwrite the files, instead just show the diffs we'd apply

        Write-Host "################################################################################" -ForegroundColor Gray
        Write-Host "###  diff: $File" -ForegroundColor Gray

        Compare-Object -ReferenceObject $OriginalContent -DifferenceObject $NewContent | ForEach-Object {
            $FontColor = $(if($_.SideIndicator -eq '=>') {'Green'} else {'Yellow'})  # Support old Powershell versions
            Write-Host "$($_.SideIndicator) $($_.InputObject)" -ForegroundColor $FontColor
        }
    }
    else
    {
        # In regular mode, overwrite the Source file with the Temp file
        Copy-Item -Path $TempFile -Destination $File -Force

        if (-not $?)
        {
            Write-Error "Failed to update $File"
            $NumErrors++
        }
    }
}

# Remove the temp file
Remove-Item -Path $TempFile 2> $null

# Prepare summary output

$MessageColor = "White"
$OptionalErrorSuffix = ""

if ($NumErrors -gt 0)
{
    $MessageColor = "Red"
    $OptionalErrorSuffix = " with $NumErrors errors"
}

$Task = "API Export"
if ($DryRun)
{
	$Task = "Dry Run"
}

Write-Host "$Task Complete$OptionalErrorSuffix" -ForegroundColor $MessageColor
