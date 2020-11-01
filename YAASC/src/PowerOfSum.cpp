#include "PowerOfSum.h"

namespace algebra {

void PowerOfSum(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal() || expr->IsFunc())
		return;

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			PowerOfSum(expr->ChildAt(i));
	}
	else
	{
		if (!expr->Left()->IsTerminal())
			PowerOfSum(expr->Left());

		if (!expr->Right()->IsTerminal())
			PowerOfSum(expr->Right());
	}

	if (!expr->IsPow())
		return;

	if (!CanApplyPowerOfSum(expr))
		return;

	// (a+b)^n
	if (!expr->Left()->IsGeneric())
		ApplyBinomialTheorem(expr);
	// (a1+a2+...am)^n
	else
		ApplyMultinomialTheorem(expr);
}

// (a+b)^n
void ApplyBinomialTheorem(std::unique_ptr<Expr>& expr) // FIXME: How about (a+b)^-n?
{
	if (expr->Right()->IsOne() || expr->Right()->IsNegOne())
		return;

	std::unique_ptr<Expr>& exponent = expr->Right();
	std::unique_ptr<Expr> add_node = std::move(expr->Left());
	std::unique_ptr<Expr> new_add_node = std::make_unique<Add>();

	int coefficient = 1;
	int n = exponent->iValue();
	int numerator = calc::Factorial(n);

	for (int i = 0; i <= n; i++)
	{
		int denominator = calc::Factorial(i) * calc::Factorial(n - i);
		coefficient = numerator / denominator;

		std::unique_ptr<Expr> mul_node = std::make_unique<Mul>();
		std::unique_ptr<Expr> expr_a;
		std::unique_ptr<Expr> expr_b;

		tree_util::Clone(expr_a, add_node->Left());
		tree_util::Clone(expr_b, add_node->Right());

		mul_node->AddChild(std::move(std::make_unique<Integer>(coefficient)));

		if (n - i > 0)
			mul_node->AddChild(std::move(std::make_unique<Pow>(std::move(expr_a), std::make_unique<Integer>(n - i))));

		if (i > 0)
			mul_node->AddChild(std::move(std::make_unique<Pow>(std::move(expr_b), std::make_unique<Integer>(i))));

		new_add_node->AddChild(std::move(mul_node));
	}

	expr = std::move(new_add_node);
}

// (a1+a2+...+am)^n
void ApplyMultinomialTheorem(std::unique_ptr<Expr>& expr)
{	
	(void)expr;
}

bool CanApplyPowerOfSum(const std::unique_ptr<Expr>& expr)
{
	// Expression is (a1+a2+...am)^n and n is integer
	if (expr->Left()->IsAdd() && expr->Right()->IsInteger())
		return true;

	return false;
}

} // namespace algebra
