#include "../flags.h"
#include <gtest/gtest.h>

class FlagOptionsTest : public ::testing::Test {
   protected:
      FlagOptions opts;

      FlagOptionsTest() : opts(std::vector<std::string>{"name", "age", "city"}) {}
};

TEST_F(FlagOptionsTest, AddAndGetCtx) {
    opts.addCtx("name", "Idan");
    EXPECT_EQ(opts.getCtx("name"), "Idan");
}

TEST_F(FlagOptionsTest, RemoveCtx) {
    opts.addCtx("age", "25");
    EXPECT_TRUE(opts.removeCtx("age"));
    EXPECT_EQ(opts.getCtx("age"), "");
    EXPECT_FALSE(opts.removeCtx("nonexistent"));
}

TEST_F(FlagOptionsTest, MissingKeyReturnsEmpty) {
    EXPECT_EQ(opts.getCtx("not_set"), "");
}

TEST_F(FlagOptionsTest, PromptOptionsParsesArgs) {
    const char* argv[] = {
        "program",
        "--name=Idan",
        "--age=30",
        "--city=TelAviv"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    opts.promptOptions(argc, const_cast<char**>(argv));

    EXPECT_EQ(opts.getCtx("name"), "Idan");
    EXPECT_EQ(opts.getCtx("age"), "30");
    EXPECT_EQ(opts.getCtx("city"), "TelAviv");
}

TEST_F(FlagOptionsTest, PromptOptionsDoesNotOverwriteExisting) {
    opts.addCtx("name", "PreSet");

    const char* argv[] = {
        "program",
        "--name=Idan"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    opts.promptOptions(argc, const_cast<char**>(argv));

    EXPECT_EQ(opts.getCtx("name"), "PreSet");
}

