#ifndef COMMON_UTILS_CONFIG_H
#define COMMON_UTILS_CONFIG_H

#include <cstdint>

namespace puzzle {

#define FILE_RELATIVE_PATH (static_cast<const char *>(__FILE__) + SOURCE_FILE_PREFIX_SIZE)

using Size = uint64_t;
using byte = unsigned char;

}  // namespace panos

#endif  // COMMON_UTILS_CONFIG_H