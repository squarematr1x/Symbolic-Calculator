#include "Calculus.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;
	
	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			Differentiate(expr->ChildAt(i));
	}
	else
	{
		if (!expr->LeftIsTerminal())
			Differentiate(expr->Left());

		if (!expr->RightIsTerminal())
			Differentiate(expr->Right());
	}

	DerivateSum(expr);
	SetToZero(expr);

	if (expr->IsZero())
		return;

	PowerRule(expr);
}

// d/dx(x^n) --> nx^(n-1)
void PowerRule(std::unique_ptr<Expr>& expr) // FIXME: x^1, 2x^4 problematic
{
	if (!expr->IsDerivative())
		return;
	
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
	std::unique_ptr<Expr> new_expr = std::make_unique<Mul>(std::move(multiplier), std::make_unique<Pow>(std::move(var),
	                                 std::make_unique<Add>(std::move(expr->Param()->Right()), std::make_unique<Integer>(-1))));

	expr = std::move(new_expr);
}

// d/dx(k) --> 0
void SetToZero(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative())
		return;

	bool is_zero = true;

	CanDifferentiate(expr, expr->Param(), is_zero);

	if (is_zero)
		expr = std::make_unique<Integer>(0);

}

void SetToOne(std::unique_ptr<Expr>& expr)
{
	(void)expr;
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

void DerivateSum(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative())
		return;

	if (!expr->Param()->IsAdd())
		return;

	if (expr->Param()->IsGeneric())
	{
		std::unique_ptr<Expr> add_node = std::make_unique<Add>();

		for (int i = 0; i < expr->Param()->ChildrenSize(); i++)
		{
			std::unique_ptr<Expr> child = std::make_unique<Derivative>(std::move(expr->Param()->ChildAt(i)), "x");
			add_node->AddChild(std::move(child));
		}

		expr = std::move(add_node);
	}
	else
	{
		std::unique_ptr<Expr> left = std::make_unique<Derivative>(std::move(expr->Param()->Left()), "x");
		std::unique_ptr<Expr> right = std::make_unique<Derivative>(std::move(expr->Param()->Right()), "x");
		expr = std::make_unique<Add>(std::move(left), std::move(right));
	}
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
