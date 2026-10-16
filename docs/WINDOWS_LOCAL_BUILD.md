# Windows integration build

The branches separate the shared Windows work from PR #79:

- `windows`: Supermedo's Windows 1.8.2 base at `fce0878`.
- `integration/deadinside28-windows`: merge deadinside28 upstream 0.51 at `3c57da4`,
  followed by its Windows renderer/runtime, input/startup and build fixes. PR #79 is excluded.
- `integration/deadinside28-pr79`: the upstream Windows branch with PR #79 at `615841c`
  merged on top, including its input/camera/prompt integration resolutions.
- `testing/full-windows-port`: the same full integration, staged for CI compilation and packaging.

The full integration's first-parent history places the upstream merge and all shared Windows
fixes before the PR #79 merge. PR-specific resolutions belong to that final merge.

## Build locally

Install MSYS2 CLANG64 and the dependencies listed in `packaging/windows/README.md`, then
initialize the submodules:

```powershell
git submodule update --init --recursive
./packaging/windows/build-local.ps1 -Msys2Root C:\msys64 -Jobs 8
```

To package the application, supply a Windows Python installation:

```powershell
./packaging/windows/build-local.ps1 -Msys2Root C:\msys64 -Jobs 8 -Package `
  -WindowsPython C:\Python313\python.exe
```

The runner uses an incremental Ninja build with LTO off. Compiler diagnostics are kept in
`out/gpu-build.log`; native binaries are in `out/`; the distributable is
`dist/bbport-windows.zip`. Build products, local configuration, and saves are excluded from Git.

The staging workflow builds the exact pushed commit. It does not fetch and merge upstream
branches or write commits. Pushes to the staging branch run one Windows packaging job; the
workflow can also be started manually. No new test cases are added by this integration.

## Local validation

The upstream-only Windows branch was compiled locally: all three application targets link,
Vulkan command submission and a 4096-byte readback pass, and `prepare.py --help` runs.

The full integration with PR #79 was compiled and packaged with MSYS2 CLANG64 Clang 23.1.3,
using the dependency list from the Windows workflow. Validation used an NVIDIA GeForce RTX 4090:

- All three Windows application targets compile and link.
- The ZIP contains the frozen launcher, native binaries and their CLANG64 DLL dependencies.
- Vulkan command submission and a 4096-byte readback pass, including from the packaged
  executable with the toolchain removed from PATH.
- The frozen launcher runs `prepare.py`; its `--run` route boots the packaged game.
- Frame-dump capture produces complete 1280x720 display and 1920x1080 output readbacks.
- Bloodborne renders the title/brightness menus and Hunter's Dream at a displayed 60 FPS.
- Keyboard button prompts render in the game's menu; F8 reloads `input.ini` live.
- Save-file writes commit successfully in an isolated copy of the user folder.
- Existing tests pass: input parser, pad input, prompt icon encoding, settings persistence,
  upscaler fallback, Python input defaults (9), FOV (3), and patches (16, one optional skip).

The existing pad test's obsolete fragments from the previous mouse API were removed so it
exercises PR #79's existing cases. Existing C/C++ tests were run with a local adapter for
Windows environment variables and assertion output. No new tests were written.

Local validation logs, the package hash manifest and screenshots are retained under `out/`.
Game validation used an isolated directory and copied saves; the installed game and BBSC
configuration were not modified.

## Current limits

- Hands-on direct mouse-camera responsiveness and automatic keyboard/controller prompt switching
  still need a playtest. The pad tests cover mouse-to-stick behavior and combinations.
- BBSC multiplayer is unverified. Windows security blocked the existing `bbsc-launch.exe` on
  the validation machine; security settings were not changed.
- The existing BBSC helper selects its bundled `emulator/bb-probe.exe`, bypassing this build.
  Its 1.7.2 relay/rejoin patch does not apply directly to the integration's newer vendored
  networking sources and has not been merged.
- Optional NVIDIA DLSS bridge/runtime and FSR 4 model assets are not included in the validated ZIP.
- Linux dma-buf memory, native guest-allocation hooks, SIGTRAP heap diagnostics and SIGPROF
  sampling are unavailable on Windows. Normal buffer copies and page tracking remain active.
- Linux builds have not been validated for this integration.
