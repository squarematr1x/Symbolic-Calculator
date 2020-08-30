#include "PowerOfSum.h"

namespace algebra {

void PowerOfSum(std::unique_ptr<Expr>& expr)
{
	if (expr->HasNoChildren())
		return;

	if (!expr->Left()->IsTerminal())
		PowerOfSum(expr->Left());

	if (!expr->Right()->IsTerminal())
		PowerOfSum(expr->Right());

	if (!expr->IsPow())
		return;

	if (!CanApplyPowerOfSum(expr))
		return;

	// (a+b)^n
	if (!expr->Left()->IsGeneric())
		ApplyBinomialTheorem(expr);
	// (a+b+...m)^n
	else
	{
		std::unique_ptr<Expr>& add_node = expr->Left();
		std::unique_ptr<Expr>& exponent = expr->Right();
		std::unique_ptr<Expr> new_add_node = std::make_unique<Mul>();

		int multiplications = stoi(exponent->Name());

		// (a+b+c)^2, in other cases the output string is inconveniently large
		if (multiplications == 2 && add_node->ChildrenSize() < 4)
		{
			for (int i = 0; i < add_node->ChildrenSize(); i++)
			{
				std::unique_ptr<Expr> copy;
				tree_util::DeepCopy(copy, add_node);
				new_add_node->AddChild(std::move(copy));
			}

			expr = std::move(new_add_node);
		}
	}
}

// (a+b)^n
void ApplyBinomialTheorem(std::unique_ptr<Expr>& expr)
{
	std::unique_ptr<Expr>& exponent = expr->Right();
	std::unique_ptr<Expr> add_node = std::move(expr->Left());
	std::unique_ptr<Expr> new_add_node = std::make_unique<Add>();

	int n = stoi(exponent->Name());
	int numerator = calc::Factorial(n);
	int coefficient = 1;

	for (int i = 0; i <= n; i++)
	{
		int denominator = calc::Factorial(i) * calc::Factorial(n - i);
		coefficient = numerator / denominator;

		std::unique_ptr<Expr> mul_node = std::make_unique<Mul>();

		std::unique_ptr<Expr> a;
		std::unique_ptr<Expr> b;
		tree_util::DeepCopy(a, add_node->Left());
		tree_util::DeepCopy(b, add_node->Right());

		mul_node->AddChild(std::move(std::make_unique<Integer>(coefficient)));

		if (n - i > 0)
			mul_node->AddChild(std::move(std::make_unique<Pow>(std::move(a), std::make_unique<Integer>(n - i))));

		if (i > 0)
			mul_node->AddChild(std::move(std::make_unique<Pow>(std::move(b), std::make_unique<Integer>(i))));

		new_add_node->AddChild(std::move(mul_node));
	}

	coefficient = 0;
	expr = std::move(new_add_node);
}

bool CanApplyPowerOfSum(const std::unique_ptr<Expr>& expr)
{
	// Expression is (a+b+...m)^n and m is integer
	if (expr->Left()->IsAdd() && expr->Right()->IsInteger())
		return true;

	return false;
}

} // namespace algebra
