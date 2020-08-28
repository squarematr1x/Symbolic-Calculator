#include "Calculus.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr, std::string respect_to)
{
	if (expr->IsTerminal())
		return;
	
	if (!expr->LeftIsTerminal())
		Differentiate(expr->Left(), respect_to);

	if (!expr->RightIsTerminal())
		Differentiate(expr->Right(), respect_to);

	if (expr->IsPow())
		PowerRule(expr, respect_to);
}

void PowerRule(std::unique_ptr<Expr>& expr, std::string respect_to) // FIXME: Just a prototype
{
	if (!expr->Left()->IsVar())
		return;

	if (expr->Left()->Name() != respect_to)
		return;

	std::unique_ptr<Expr> multiplier;
	tree_util::DeepCopy(multiplier, expr->Right());

	expr = std::make_unique<Pow>(std::make_unique<Mul>(std::move(multiplier), std::move(expr->Left())),
	       std::make_unique<Add>(std::move(expr->Right()), std::make_unique<Integer>(-1)));
}

bool HasPowChildren(std::unique_ptr<Expr>& expr)
{
	if (expr->Left()->IsPow() && expr->Right()->IsPow())
		return true;

	return false;
}


void Integrate(std::unique_ptr<Expr>& expr, std::string respect_to)
{
	// Apply integration rules
}

}
