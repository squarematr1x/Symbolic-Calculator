#include "Calculus.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;
	
	if (!expr->LeftIsTerminal())
		Differentiate(expr->Left());

	if (!expr->RightIsTerminal())
		Differentiate(expr->Right());

	if (!expr->IsDerivative())
		return;

	SetToZero(expr);

	if (!expr->IsZero())
		PowerRule(expr);
}

// D(x^n) --> nx^(n-1)
void PowerRule(std::unique_ptr<Expr>& expr) // FIXME: x^1 problematic
{
	if (!expr->Param()->IsPow())
		return;

	if (!expr->Param()->Left()->IsPow())
		return;

	if (!expr->Param()->Left()->Left()->IsVar())
		return;

	if (expr->Param()->Left()->Left()->Name() != expr->RespectTo())
		return;

	std::unique_ptr<Expr> multiplier;
	tree_util::DeepCopy(multiplier, expr->Param()->Right());
	std::unique_ptr<Expr> var = std::move(expr->Param()->Left()->Left());

	std::unique_ptr<Expr> new_expr = std::make_unique<Pow>(std::make_unique<Mul>(std::move(multiplier), std::move(var)),
	                                 std::make_unique<Add>(std::move(expr->Param()->Right()), std::make_unique<Integer>(-1)));

	expr = std::move(new_expr);
}

// Dk --> 0
void SetToZero(std::unique_ptr<Expr>& expr)
{
	bool is_zero = true;

	CanDifferentiate(expr, expr->Param(), is_zero);

	if (is_zero)
		expr = std::make_unique<Integer>(0);

}

void CanDifferentiate(const std::unique_ptr<Expr>& root, const std::unique_ptr<Expr>& expr, bool& is_constant)
{
	if (!is_constant)
		return;

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			CanDifferentiate(root, expr->ChildAt(i), is_constant);
	}
	else
	{
		if (expr->HasLeftChild())
			CanDifferentiate(root, expr->Left(), is_constant);
		if (expr->HasRightChild())
			CanDifferentiate(root, expr->Right(), is_constant);
	}

	if (root->RespectTo() == expr->Name())
		is_constant = false;
}

bool HasPowChildren(std::unique_ptr<Expr>& expr)
{
	if (expr->Left()->IsPow() && expr->Right()->IsPow())
		return true;

	return false;
}


void Integrate(std::unique_ptr<Expr>& expr)
{
	(void)expr;
	// Apply integration rules
}

}
