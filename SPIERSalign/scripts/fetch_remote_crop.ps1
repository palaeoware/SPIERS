param(
    [Parameter(Mandatory = $true)]
    [string]$RemoteDir,

    [Parameter(Mandatory = $true)]
    [string]$LocalDir,

    [Parameter(Mandatory = $true)]
    [int]$FirstFileNumber,

    [Parameter(Mandatory = $true)]
    [int]$LastFileNumber,

    [Parameter(Mandatory = $true)]
    [int]$CropX,

    [Parameter(Mandatory = $true)]
    [int]$CropY,

    [Parameter(Mandatory = $true)]
    [int]$CropWidth,

    [Parameter(Mandatory = $true)]
    [int]$CropHeight,

    [ValidateRange(1, 8)]
    [int]$CoordinateBinning = 2,

    [ValidateRange(0, 65535)]
    [int]$WindowLow = 27000,

    [ValidateRange(0, 65535)]
    [int]$WindowHigh = 39000,

    [ValidateRange(1, 999999)]
    [int]$Step = 1,

    [ValidateRange(1, 8)]
    [int]$ConversionWorkers = 2,

    [ValidateRange(1, 8)]
    [int]$OutputBinning = 1,

    [ValidateSet("bmp", "jpg")]
    [string]$OutputFormat = "bmp",

    [ValidateSet("true", "false")]
    [string]$DeleteJp2 = "true",

    [string]$Password = "",

    [ValidateSet("true", "false")]
    [string]$DeleteScriptAfterLaunch = "false",

    [ValidateSet("true", "false")]
    [string]$TestMode = "false"
)

$ErrorActionPreference = "Stop"

trap {
    $message = $_.Exception.Message
    if ($DeleteScriptAfterLaunch -eq "true") {
        Remove-Item -LiteralPath $PSCommandPath -Force -ErrorAction SilentlyContinue
    }
    Write-Error $message -ErrorAction Continue
    exit 1
}

# These connection details are intentionally fixed for the dedicated SPIERSalign workflow.
$winscpDll = "C:\Program Files (x86)\WinSCP\WinSCPnet.dll"
$hostName = "ese-marknas.ese.imperial.ac.uk"
$userName = "ESRFdata"
$hostKeyFingerprint = "ssh-ed25519 255 /lCYF3DppEmLWD3c5fQd53lkJP15l6xhidoWY5oNSeQ"
$magickExe = "magick"
$jpegQuality = 95

function Write-SpiersStatus([string]$Message) {
    Write-Output "SPIERSALIGN_STATUS|$Message"
}

function Complete-NextConversion(
    [ref]$ActiveConversions,
    [ref]$ConvertedCount,
    [int]$TotalCount,
    [bool]$DeleteDownloadedJp2
) {
    $record = $null
    while ($null -eq $record) {
        foreach ($candidate in @($ActiveConversions.Value)) {
            if ($candidate.AsyncResult.IsCompleted) {
                $record = $candidate
                break
            }
        }
        if ($null -eq $record) {
            Start-Sleep -Milliseconds 50
        }
    }

    $failure = $null
    $result = $null
    try {
        $results = @($record.PowerShell.EndInvoke($record.AsyncResult))
        $result = $results | Select-Object -Last 1

        if ($record.PowerShell.Streams.Error.Count -gt 0) {
            $failure = ($record.PowerShell.Streams.Error | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine
        }
        elseif ($null -eq $result) {
            $failure = "The conversion worker returned no result."
        }
        elseif ([int]$result.ExitCode -ne 0) {
            $failure = [string]$result.Output
            if ([string]::IsNullOrWhiteSpace($failure)) {
                $failure = "ImageMagick exited with code $($result.ExitCode)."
            }
        }
    }
    catch {
        $failure = $_.Exception.Message
    }
    finally {
        $record.PowerShell.Dispose()
        $ActiveConversions.Value = @(
            $ActiveConversions.Value | Where-Object { $_ -ne $record }
        )
    }

    if (-not [string]::IsNullOrWhiteSpace([string]$result.Output)) {
        Write-Output ([string]$result.Output).Trim()
    }
    if (-not [string]::IsNullOrWhiteSpace($failure)) {
        throw "ImageMagick failed while converting '$($record.FileName)': $failure"
    }

    if ($DeleteDownloadedJp2) {
        Remove-Item -LiteralPath $record.LocalJp2
    }

    $ConvertedCount.Value = [int]$ConvertedCount.Value + 1
    Write-Output "SPIERSALIGN_PROGRESS|$($ConvertedCount.Value)|$TotalCount|$($record.FileName)"
}

if ($LastFileNumber -lt $FirstFileNumber) {
    throw "The last file number must be greater than or equal to the first file number."
}
if ($WindowHigh -le $WindowLow) {
    throw "Window high must be greater than window low."
}
if ($CropX -lt 0 -or $CropY -lt 0 -or $CropWidth -lt 1 -or $CropHeight -lt 1) {
    throw "The crop rectangle is invalid."
}
if (-not (Test-Path -LiteralPath $winscpDll -PathType Leaf)) {
    throw "WinSCP .NET assembly not found at '$winscpDll'."
}
$magickCommand = Get-Command $magickExe -ErrorAction SilentlyContinue
if (-not $magickCommand) {
    throw "ImageMagick executable '$magickExe' was not found on PATH."
}
$magickExe = $magickCommand.Source

$password = $Password
if ([string]::IsNullOrEmpty($password)) {
    $password = [Environment]::GetEnvironmentVariable("SPIERSALIGN_SFTP_PASSWORD", "Process")
}
if ([string]::IsNullOrEmpty($password)) {
    throw "No SFTP password was supplied."
}

if (-not (Test-Path -LiteralPath $LocalDir -PathType Container)) {
    New-Item -ItemType Directory -Path $LocalDir -Force | Out-Null
}

Add-Type -Path $winscpDll

$sessionOptions = New-Object WinSCP.SessionOptions
$sessionOptions.Protocol = [WinSCP.Protocol]::Sftp
$sessionOptions.HostName = $hostName
$sessionOptions.UserName = $userName
$sessionOptions.Password = $password
$sessionOptions.SshHostKeyFingerprint = $hostKeyFingerprint

$session = New-Object WinSCP.Session
$runspacePool = $null
$activeConversions = @()
$savedMagickThreadLimit = [Environment]::GetEnvironmentVariable("MAGICK_THREAD_LIMIT", "Process")

try {
    Write-SpiersStatus "Connecting to $hostName"
    $session.Open($sessionOptions)

    Write-SpiersStatus "Reading remote directory"
    $files = @(
        $session.ListDirectory($RemoteDir).Files |
            Where-Object {
                if ($_.IsDirectory -or -not $_.Name.EndsWith(".jp2", [System.StringComparison]::OrdinalIgnoreCase)) {
                    return $false
                }

                $match = [regex]::Match(
                    [System.IO.Path]::GetFileNameWithoutExtension($_.Name),
                    "(\d{6})$"
                )
                if (-not $match.Success) {
                    return $false
                }

                $number = [int]$match.Groups[1].Value
                return $number -ge $FirstFileNumber -and
                    $number -le $LastFileNumber -and
                    (($number - $FirstFileNumber) % $Step) -eq 0
            } |
            Sort-Object @{
                Expression = {
                    $match = [regex]::Match(
                        [System.IO.Path]::GetFileNameWithoutExtension($_.Name),
                        "(\d{6})$"
                    )
                    [int]$match.Groups[1].Value
                }
            }, Name
    )

    if ($files.Count -eq 0) {
        throw "No JP2 files ending in six digits were found in the requested number range $FirstFileNumber-$LastFileNumber with step $Step."
    }

    if ($TestMode -eq "true") {
        $testIndices = @(
            0
            [int][Math]::Floor(($files.Count - 1) / 2)
            $files.Count - 1
        ) | Select-Object -Unique
        $files = @($testIndices | ForEach-Object { $files[$_] })
        Write-SpiersStatus "Test mode selected $($files.Count) image(s)"
    }

    $cropXAtSource = $CropX * $CoordinateBinning
    $cropYAtSource = $CropY * $CoordinateBinning
    $cropWidthAtSource = $CropWidth * $CoordinateBinning
    $cropHeightAtSource = $CropHeight * $CoordinateBinning
    $cropGeometry = "${cropWidthAtSource}x${cropHeightAtSource}+${cropXAtSource}+${cropYAtSource}"
    $deleteDownloadedJp2 = $DeleteJp2 -eq "true"
    $downloaded = 0
    $converted = 0
    $threadsPerWorker = [Math]::Max(1, [int][Math]::Floor([Environment]::ProcessorCount / $ConversionWorkers))
    [Environment]::SetEnvironmentVariable("MAGICK_THREAD_LIMIT", [string]$threadsPerWorker, "Process")

    $conversionWorkerScript = {
        param($workItem)

        $arguments = @($workItem.Arguments)
        $nativeOutput = @(
            & $workItem.Executable @arguments 2>&1 |
                ForEach-Object { $_.ToString() }
        )
        [pscustomobject]@{
            ExitCode = $LASTEXITCODE
            Output = $nativeOutput -join [Environment]::NewLine
        }
    }

    $runspacePool = [RunspaceFactory]::CreateRunspacePool(1, $ConversionWorkers)
    $runspacePool.Open()
    Write-SpiersStatus "Using $ConversionWorkers conversion worker(s), with up to $threadsPerWorker ImageMagick thread(s) each"

    foreach ($file in $files) {
        $remotePath = [WinSCP.RemotePath]::Combine($RemoteDir, $file.Name)
        $localJp2 = Join-Path $LocalDir $file.Name
        $baseName = [System.IO.Path]::GetFileNameWithoutExtension($file.Name)
        $localOut = Join-Path $LocalDir ($baseName + "." + $OutputFormat)

        Write-SpiersStatus "Downloading $($file.Name)"
        $transferResult = $session.GetFiles($remotePath, $localJp2, $false)
        $transferResult.Check()
        $downloaded++
        Write-Output "SPIERSALIGN_DOWNLOAD|$downloaded|$($files.Count)|$($file.Name)|$($activeConversions.Count)"

        # One file may be downloaded ahead of the pool. This overlaps the sequential
        # download with all active conversions without allowing an unbounded JP2 queue.
        while ($activeConversions.Count -ge $ConversionWorkers) {
            Complete-NextConversion `
                -ActiveConversions ([ref]$activeConversions) `
                -ConvertedCount ([ref]$converted) `
                -TotalCount $files.Count `
                -DeleteDownloadedJp2 $deleteDownloadedJp2
        }

        $magickArgs = @(
            $localJp2,
            "-gravity", "NorthWest",
            "-crop", $cropGeometry,
            "+repage",
            "-gravity", "None",
            "-colorspace", "Gray",
            "-level", "$WindowLow,$WindowHigh",
            "-depth", "8",
            "-strip"
        )

        if ($OutputBinning -gt 1) {
            $resizePercent = 100.0 / $OutputBinning
            $magickArgs += @(
                "-filter", "box",
                "-resize", ($resizePercent.ToString("0.########", [System.Globalization.CultureInfo]::InvariantCulture) + "%")
            )
        }

        if ($OutputFormat -eq "jpg") {
            $magickArgs += @(
                "-type", "Grayscale",
                "-quality", "$jpegQuality",
                $localOut
            )
        }
        else {
            $magickArgs += @(
                "-type", "Palette",
                "-colors", "256",
                "-compress", "none",
                "BMP3:$localOut"
            )
        }

        $workItem = @{
            Executable = $magickExe
            Arguments = [string[]]$magickArgs
        }
        $worker = [PowerShell]::Create()
        $worker.RunspacePool = $runspacePool
        $null = $worker.AddScript($conversionWorkerScript.ToString()).AddArgument($workItem)
        $asyncResult = $worker.BeginInvoke()
        $activeConversions += [pscustomobject]@{
            PowerShell = $worker
            AsyncResult = $asyncResult
            FileName = $file.Name
            LocalJp2 = $localJp2
        }
        Write-SpiersStatus "Converting $($file.Name) ($($activeConversions.Count) active)"
    }

    while ($activeConversions.Count -gt 0) {
        Complete-NextConversion `
            -ActiveConversions ([ref]$activeConversions) `
            -ConvertedCount ([ref]$converted) `
            -TotalCount $files.Count `
            -DeleteDownloadedJp2 $deleteDownloadedJp2
    }

    Write-Output "SPIERSALIGN_COMPLETE|$converted|$LocalDir"
}
finally {
    foreach ($record in @($activeConversions)) {
        try {
            $record.PowerShell.Stop()
        }
        catch {
            # Best-effort cleanup; the original conversion error is more useful.
        }
        $record.PowerShell.Dispose()
    }
    if ($null -ne $runspacePool) {
        $runspacePool.Close()
        $runspacePool.Dispose()
    }
    [Environment]::SetEnvironmentVariable("MAGICK_THREAD_LIMIT", $savedMagickThreadLimit, "Process")
    $session.Dispose()
    [Environment]::SetEnvironmentVariable("SPIERSALIGN_SFTP_PASSWORD", $null, "Process")
    if ($DeleteScriptAfterLaunch -eq "true") {
        Remove-Item -LiteralPath $PSCommandPath -Force -ErrorAction SilentlyContinue
    }
}
