#include "common/utils/log.h"
#include "gtest/gtest.h"

namespace puzzle {

class LogTest : public ::testing::Test {};

TEST_F(LogTest, BacktraceStringTest) {
    Log::AppendLog("abc", 3);
}

TEST_F(LogTest, AssertTest) {

}

}  // namespace panos
