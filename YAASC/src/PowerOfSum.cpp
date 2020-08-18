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

	std::unique_ptr<Expr>& add_node = expr->Left();
	std::unique_ptr<Expr>& exponent = expr->Right();

	// (a+b)^n
	if (!add_node->IsGeneric())
	{
		int multiplications = stoi(exponent->Name());

		// (a+b)^2
		if (multiplications == 2)
		{
			std::unique_ptr<Expr> copy;
			tree_util::DeepCopy(copy, add_node);
			expr = std::make_unique<Mul>(std::move(add_node), std::move(copy));
		}
		// (a+b)^n, where n > 2
		else if (multiplications > 2)
		{
			std::unique_ptr<Expr> new_add_node = std::make_unique<Mul>();

			for (int i = 0; i < multiplications; i++)
			{
				std::unique_ptr<Expr> copy;
				tree_util::DeepCopy(copy, add_node);
				new_add_node->AddChild(std::move(copy));
			}

			expr = std::move(new_add_node);
		}
	}
	// (a+b+...m)^n
	else
	{
		// Implement generic case
	}
}

bool CanApplyPowerOfSum(const std::unique_ptr<Expr>& expr)
{
	// Expression is (a+b+...m)^n and m is integer
	if (expr->Left()->IsAdd() && expr->Right()->IsInteger())
		return true;

	return false;
}

}
