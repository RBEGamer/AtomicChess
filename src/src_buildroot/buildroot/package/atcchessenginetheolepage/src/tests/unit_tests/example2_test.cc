#include "gtest/gtest.h"

#include <queue>

double sqrt(double num)
{
    if (num < 0.0)
    {
        std::cerr << "Error: Negative Input\n";
        exit(1);
    }
    return 0.0f;
}

TEST(Testname, testcae)
{
    // ASSERT_* stops right away
    // EXPECT_* continues, can see more error

    // Equal
    EXPECT_EQ(1, 1);
    EXPECT_FLOAT_EQ(0.0f, 0.0f);
    EXPECT_DOUBLE_EQ(0.0, 0.0);

    // Different
    EXPECT_EQ(1, 1); // val1 == val2
    EXPECT_NE(1, 2); // val1 != val2
    EXPECT_LT(1, 2); // val1 < val2
    EXPECT_LE(1, 1); // val1 <= val2
    EXPECT_GT(2, 1); // val1 > val2
    EXPECT_GE(2, 2); // val1 >= val2

    // String
    std::string a("hello"), b("hello");
    EXPECT_STREQ("hello","hello");     //the two C strings have the same content
    EXPECT_STRNE("hello","Hello");     //the two C strings have different contents
    EXPECT_STRCASEEQ("hello","Hello"); //the two C strings have the same content, ignoring case
    EXPECT_STRCASENE("hello","helloo"); //the two C strings have different contents, ignoring case
    EXPECT_EQ(a, b); //the two C++ string have the same content

    // Return status & error message
    ASSERT_EXIT(sqrt(-22.0),
                ::testing::ExitedWithCode(1),
                "Error: Negative Input");

}

// To start tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}