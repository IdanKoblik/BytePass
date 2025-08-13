#include <gtest/gtest.h>
#include "../prompt.h"

class PromptOptionsTest : public ::testing::Test {
   protected:
      PromptOptions opts;

      PromptOptionsTest() : opts({"name", "age", "city"}) {}
};

TEST_F(PromptOptionsTest, ReadsValuesFromCin) {
    // Simulate user pressing Enter to skip the initial std::cin.ignore()
    // then typing 3 answers: Alice, 30, Tel Aviv
    std::istringstream input("\nAlice\n30\nTel Aviv\n");

    // Redirect std::cin to read from our fake input
    auto cinbuf = std::cin.rdbuf(input.rdbuf());

    const char* argv[] = {"program"};
    int argc = 1;

    opts.promptOptions(argc, const_cast<char**>(argv));

    // Restore cin buffer
    std::cin.rdbuf(cinbuf);

    EXPECT_EQ(opts.getCtx("name"), "Alice");
    EXPECT_EQ(opts.getCtx("age"), "30");
    EXPECT_EQ(opts.getCtx("city"), "Tel Aviv");
}

TEST_F(PromptOptionsTest, HandlesEmptyInputs) {
    std::istringstream input("\n\n\n\n"); // skip + 3 empty answers
    auto cinbuf = std::cin.rdbuf(input.rdbuf());

    const char* argv[] = {"program"};
    int argc = 1;

    opts.promptOptions(argc, const_cast<char**>(argv));

    std::cin.rdbuf(cinbuf);

    EXPECT_EQ(opts.getCtx("name"), "");
    EXPECT_EQ(opts.getCtx("age"), "");
    EXPECT_EQ(opts.getCtx("city"), "");
}
