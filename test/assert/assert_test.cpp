#include "common/utils/assert.h"
#include "gtest/gtest.h"

namespace puzzle {

std::string Bar() {
    return GetBacktraceString();
}

std::string Foo() {
    return Bar();
}

class AssertTest : public ::testing::Test {};

TEST_F(AssertTest, BacktraceStringTest) {
    ASSERT_NE(Foo(), "");
}

TEST_F(AssertTest, AssertTest) {
    // Assert(1 == 2, "some information");
    Assert(2 == 2);
}

}  // namespace panos
