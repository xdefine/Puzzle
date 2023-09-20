#ifndef COMMON_UTILS_ASSERT_H
#define COMMON_UTILS_ASSERT_H

#include <cstdarg>
#include <iostream>
#include <string>

#include "config.h"

namespace panos {

std::string GetBacktraceString(int max_frame_num = 128, int skip = 2, const std::string &prefix = "");

#define Assert(condition, ...)                                                                                         \
    if (!(condition)) {                                                                                                \
        ::printf("Assert at %s:%d fail. %s\n", FILE_RELATIVE_PATH, __LINE__, __VA_ARGS__ "");                          \
        std::cout << GetBacktraceString() << std::flush;                                                               \
        exit(-1);                                                                                                      \
    }

}  // namespace panos

#endif  // COMMON_UTILS_ASSERT_H