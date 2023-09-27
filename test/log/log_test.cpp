#include "common/utils/log.h"
#include "gtest/gtest.h"

namespace puzzle {

class LogTest : public ::testing::Test {};

TEST_F(LogTest, LogTest) {
    LOG_DEBUG(Module::Common, "format");
    LOG_INFO(Module::Common, "format: %d", 10);
    LOG_WARN(Module::Common, "%d, %f, %s", 10, 0.5, "abc");
    LOG_ERROR(Module::Common, "=== %d : %d ===", 500, 600);
    Log::StopAndWaitFlushComplete();
}

}  // namespace panos
