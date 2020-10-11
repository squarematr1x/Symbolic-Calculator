#include <gtest/gtest.h>

#include "../src/Scanner.h"

namespace scanner {

TEST(TestCleanInput, StringInput)
{
	std::string input_a = "3 +5* ah + %%_,3";
	std::string input_b = "x+++y **&17";
	std::string input_c = "";

	CleanInput(input_a);
	CleanInput(input_b);
	CleanInput(input_c);

	std::string expected_a = "3+5*ah+3";
	std::string expected_b = "x+y*17";
	std::string expected_c = "";

	EXPECT_TRUE(expected_a == input_a);
	EXPECT_TRUE(expected_b == input_b);
	EXPECT_TRUE(expected_c == input_c);
}

TEST(TestAddMultiplySign, StringInput)
{
	std::string input_a = "35a4+cc";
	std::string input_b = "x*5+17.5x";

	AddMultiplySign(input_a);
	AddMultiplySign(input_b);

	std::string expected_a = "35*a*4+c*c";
	std::string expected_b = "x*5+17.5*x";

	EXPECT_TRUE(expected_a == input_a);
	EXPECT_TRUE(expected_b == input_b);
}

TEST(TestAddUnaryToken, StringInput)
{
	std::string input_a = "2-4+x-7";
	std::string input_b = "2^-4+x-7";
	std::string input_c = "y!-5+x*-z+-u";

	AddUnaryToken(input_a);
	AddUnaryToken(input_b);
	AddUnaryToken(input_c);

	std::string expected_a = "2-4+x-7";
	std::string expected_b = "2^_4+x-7";
	std::string expected_c = "y!-5+x*_z+_u";

	EXPECT_TRUE(expected_a == input_a);
	EXPECT_TRUE(expected_b == input_b);
	EXPECT_TRUE(expected_c == input_c);
}

TEST(TestMissingParenthesis, StringInput)
{
	std::string input_a = "4(5+x))*y";
	std::string input_b = "4(5+x)*(y";
	std::string input_c = "a((9x)sin(x))";
	std::string input_d = "x+y+z-4abc";

	EXPECT_TRUE(MissingParenthesis(input_a));
	EXPECT_TRUE(MissingParenthesis(input_b));
	EXPECT_FALSE(MissingParenthesis(input_c));
	EXPECT_FALSE(MissingParenthesis(input_d));
}

TEST(TestAddFunctionToken, StringInput)
{
	std::string input_a = "3+sin(x)+4.5+log(2)";
	std::string input_b = "5!-4.5+D(x^2)/cos(3)";
	std::string input_c = "xx+z+san(y)-7+cis(x)";

	AddFunctionToken(input_a);
	AddFunctionToken(input_b);
	AddFunctionToken(input_c);

	std::string expected_a = "3+#sin(x)+4.5+#log(2)";
	std::string expected_b = "5!-4.5+#D(x^2)/#cos(3)";
	std::string expected_c = "xx+z+san(y)-7+cis(x)";

	EXPECT_TRUE(expected_a == input_a);
	EXPECT_TRUE(expected_b == input_b);
	EXPECT_TRUE(expected_c == input_c);
}

TEST(TestPrecedence, OperatorInput)
{
	EXPECT_EQ(1, Precedence('+'));
	EXPECT_EQ(2, Precedence('*'));
	EXPECT_EQ(-1, Precedence('5'));
}

}
