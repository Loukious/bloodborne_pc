param(
    [string]$Msys2Root = $env:MSYS2_ROOT,
    [string]$WindowsPython = $env:WINPYTHON,
    [ValidateRange(1, 128)][int]$Jobs = 8,
    [switch]$Package
)
$ErrorActionPreference = 'Stop'
if (-not $Msys2Root) { $Msys2Root = 'C:\msys64' }
$Msys2Root = (Resolve-Path -LiteralPath $Msys2Root).Path
$bash = Join-Path $Msys2Root 'usr\bin\bash.exe'
if (-not (Test-Path -LiteralPath $bash)) { throw "MSYS2 bash missing: $bash" }
$root = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$env:MSYSTEM = 'CLANG64'
$env:CHERE_INVOKING = '1'
$env:MSYS2_ROOT = $Msys2Root.Replace('\', '/')
$env:BB_BUILD_JOBS = "$Jobs"
$env:BB_LTO = 'OFF'
if ($Package) {
    if (-not $WindowsPython) {
        $WindowsPython = (Get-Command python.exe -ErrorAction SilentlyContinue).Source
    }
    if (-not $WindowsPython -or -not (Test-Path -LiteralPath $WindowsPython)) {
        throw 'Packaging needs a Windows Python. Pass -WindowsPython C:\path\to\python.exe.'
    }
    $env:WINPYTHON = (Resolve-Path -LiteralPath $WindowsPython).Path.Replace('\', '/')
}
Push-Location -LiteralPath $root
try {
    & $bash -lc 'bash build.sh'
    if ($LASTEXITCODE -ne 0) { throw 'Build failed. See out/gpu-build.log.' }
    if ($Package) {
        & $bash -lc 'bash packaging/windows/package.sh'
        if ($LASTEXITCODE -ne 0) { throw 'Packaging failed.' }
    }
} finally {
    Pop-Location
}
