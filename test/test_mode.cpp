#include <gtest/gtest.h>
#include <stdexcept>
#include "../mode.h"

TEST(ParseModeTests, ValidName) {
   EXPECT_EQ(parseMode("receiver"), RECEIVER);
   EXPECT_EQ(parseMode("sender"), SENDER);
}

TEST(ParseModeTests, InvalidName) {
   EXPECT_THROW(parseMode("invalid"), std::runtime_error);
}
