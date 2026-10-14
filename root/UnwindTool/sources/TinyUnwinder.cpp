#include "TinyUnwinder.h"

#include <cxxabi.h>
#include <dlfcn.h>
#include <libunwind.h>
#include <link.h>

#include <memory>

namespace Tools::Unwind {
template<typename... Args>
static std::string string_format(const std::string& format, Args... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0) {
        return "";
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

std::string TinyUnwinder::GetStackBacktraceStr() const
{
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;

    unw_getcontext(&uc);
    unw_init_local(&cursor, &uc);
    unw_word_t offset;
    char sym[16384];
    int32_t stackFrameIndex{0};
    std::string btStr = "\n";
    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        // printf("ip = %lx, sp = %lx\n", (long)ip, (long)sp);
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            // Demangle C++ symbol names
            int status = 0;
            char* demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
            const char* name = (status == 0 && demangled) ? demangled : sym;

            // Get base address to calculate relative offset for addr2line
            Dl_info dlinfo;
            unw_word_t relativeAddr = ip;
            if (dladdr((void*)ip, &dlinfo) && dlinfo.dli_fbase) {
                relativeAddr = ip - (unw_word_t)dlinfo.dli_fbase;
            }

            // Include absolute IP, relative offset, and function offset
            btStr += string_format(
                "#%d [0x%lx] (0x%lx) (%s+0x%lx)\n",
                stackFrameIndex++,
                (unsigned long)ip,
                (unsigned long)relativeAddr,
                name,
                offset);

            if (demangled) {
                free(demangled);
            }
        }
    }

    return btStr;
}
} // namespace Tools::Unwind