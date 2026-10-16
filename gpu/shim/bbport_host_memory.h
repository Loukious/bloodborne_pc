// SPDX-License-Identifier: GPL-2.0-or-later
// Memory for native detours near the guest image. Guest callbacks use the PS4's ABI.
#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#define BB_GUEST_ABI __attribute__((sysv_abi))
#else
#include <sys/mman.h>
#include <unistd.h>
#define BB_GUEST_ABI
#endif

namespace BbHostMemory {
inline void* AlignedAlloc(std::size_t alignment, std::size_t size) {
#ifdef _WIN32
    return _aligned_malloc(size, alignment);
#else
    return std::aligned_alloc(alignment, size);
#endif
}
inline void AlignedFree(void* pointer) {
#ifdef _WIN32
    _aligned_free(pointer);
#else
    std::free(pointer);
#endif
}
inline constexpr int Read = 1, Write = 2, Execute = 4;
inline std::size_t PageSize() {
#ifdef _WIN32
    SYSTEM_INFO info{};
    GetSystemInfo(&info);
    return info.dwPageSize;
#else
    return static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
#endif
}
inline void* MapAt(std::uintptr_t hint, std::size_t size, int protection) {
#ifdef _WIN32
    const DWORD flags = protection & Execute ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
    return VirtualAlloc(reinterpret_cast<void*>(hint), size, MEM_RESERVE | MEM_COMMIT, flags);
#else
    void* result = mmap(reinterpret_cast<void*>(hint), size, protection,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    return result == MAP_FAILED ? nullptr : result;
#endif
}
inline bool Protect(void* address, std::size_t size, int protection) {
#ifdef _WIN32
    DWORD previous{};
    const DWORD flags = protection & Execute
        ? (protection & Write ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ)
        : (protection & Write ? PAGE_READWRITE : protection & Read ? PAGE_READONLY : PAGE_NOACCESS);
    if (!VirtualProtect(address, size, flags, &previous)) { return false; }
    if (protection & Execute) { FlushInstructionCache(GetCurrentProcess(), address, size); }
    return true;
#else
    return mprotect(address, size, protection) == 0;
#endif
}
} // namespace BbHostMemory
