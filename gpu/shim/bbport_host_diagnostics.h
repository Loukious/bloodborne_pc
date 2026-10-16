// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#ifdef _WIN32
#include <windows.h>
#include <cstdio>
#include <cstdint>
struct Dl_info {
    const char* dli_fname{};
    void* dli_fbase{};
    const char* dli_sname{};
    void* dli_saddr{};
};
inline int dladdr(const void* address, Dl_info* info) {
    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory))) { return 0; }
    thread_local char module[MAX_PATH];
    if (!GetModuleFileNameA(static_cast<HMODULE>(memory.AllocationBase), module, MAX_PATH)) {
        return 0;
    }
    *info = {module, memory.AllocationBase, nullptr, nullptr};
    return 1;
}
inline int backtrace(void** frames, int count) {
    return CaptureStackBackTrace(1, static_cast<DWORD>(count), frames, nullptr);
}
inline void backtrace_symbols_fd(void* const* frames, int count, int) {
    for (int i = 0; i < count; ++i) {
        Dl_info info{};
        dladdr(frames[i], &info);
        std::fprintf(stderr, "  %s+%#llx\n", info.dli_fname ? info.dli_fname : "?",
            static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(frames[i]) -
                                           reinterpret_cast<std::uintptr_t>(info.dli_fbase)));
    }
}
#else
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>
#endif
