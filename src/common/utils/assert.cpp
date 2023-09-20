#include <cxxabi.h>
#include <execinfo.h>
#include <sstream>

#include "common/utils/assert.h"

namespace puzzle {

static std::string Demangle(const char *str) {
    size_t size = 0;
    int status = 0;
    char buf[256];
    if (sscanf(str, "%*[^(]%*[^_]%255[^)+]", buf) == 1) {
        char *v = abi::__cxa_demangle(buf, nullptr, &size, &status);
        if (v != nullptr) {
            std::string result(v);
            free(v);
            return result;
        }
    }
    if (sscanf(str, "%255s", buf)) {
        return buf;
    } else {
        return str;
    }
}

std::string GetBacktraceString(int max_frame_num, int skip, const std::string &prefix) {
    void **callstack = static_cast<void **>(malloc(sizeof(void *) * max_frame_num));
    int frame_num = ::backtrace(callstack, max_frame_num);
    char **symbols_strings = ::backtrace_symbols(callstack, frame_num);
    if (symbols_strings == nullptr) {
        return {};
    }
    std::ostringstream trace_buf;
    for (int i = skip; i < frame_num; ++i) {
        trace_buf << prefix << " " << Demangle(symbols_strings[i]) << "\n";
    }
    free(symbols_strings);
    free(callstack);
    return trace_buf.str();
}

}  // namespace panos