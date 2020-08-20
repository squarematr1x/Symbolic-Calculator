#include "Calculus.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr, std::string respect_to)
{
	// Apply differentiation rules
}

void PowerRule(std::unique_ptr<Expr>& expr, std::string respect_to) // FIXME: Just a prototype
{
	if (!expr->IsPow())
		return;

	if (!expr->Left()->IsVar())
		return;

	if (expr->Left()->Name() != respect_to)
		return;

	std::unique_ptr<Expr> multiplier;
	tree_util::DeepCopy(multiplier, expr->Right());

	expr = std::make_unique<Pow>(std::make_unique<Mul>(std::move(multiplier), std::move(expr->Left())),
	       std::make_unique<Add>(std::move(expr->Right()), std::make_unique<Integer>(-1)));
}

void Integrate(std::unique_ptr<Expr>& expr, std::string respect_to)
{
	// Apply integration rules
}

}
