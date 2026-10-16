# Sourced by the Windows build/run scripts: puts the MSYS2 CLANG64 toolchain first on PATH.
# Keep the active shell's installation: CI can also have an older C:/msys64 installed.
# Mixing their usr/bin tools makes /clang64 refer to different Windows directories.
if [[ -n ${MSYS2_ROOT:-} ]]; then
    msys2_root=$(cygpath -u "$MSYS2_ROOT")
elif [[ -d /clang64/bin ]]; then
    msys2_root=/
else
    msys2_root=/c/msys64
fi
msys2_root=${msys2_root%/}
if [[ -d $msys2_root/clang64/bin ]]; then
    export PATH="$msys2_root/clang64/bin:$msys2_root/usr/bin:$PATH"
    export MSYSTEM=CLANG64
fi
