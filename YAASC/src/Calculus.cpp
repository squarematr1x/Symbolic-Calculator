#include "Calculus.h"
#include "TreeUtil.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;

	if (expr->IsFunc())
		Differentiate(expr->Param());
	else if (expr->IsGeneric())
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

	ApplyDerivativeRules(expr);
}

// d/dx(x^n) --> nx^(n-1)
void PowerRule(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative()) // FIXME: Trim these?
		return;
	
	if (!expr->Param()->IsPow())
		return;

	// D(x^1), FIXME: probably not final
	if (expr->Param()->Right()->IsOne())
	{
		expr = std::make_unique<Integer>(1);
		return;
	}

	if (!expr->Param()->Left()->IsPow())
		return;

	if (!expr->Param()->Left()->Left()->IsVar())
		return;

	if (expr->Param()->Left()->Left()->Name() != expr->RespectTo())
		return;

	std::unique_ptr<Expr> multiplier;
	tree_util::Clone(multiplier, expr->Param()->Right());
	std::unique_ptr<Expr> var = std::move(expr->Param()->Left()->Left());
	std::unique_ptr<Expr> new_expr = std::make_unique<Mul>(std::move(multiplier), std::make_unique<Pow>(std::move(var),
	                                 std::make_unique<Add>(std::move(expr->Param()->Right()), std::make_unique<Integer>(-1))));

	expr = std::move(new_expr);
}

void ExponentialRule(std::unique_ptr<Expr>& expr)
{
	std::unique_ptr<Expr> base;
	tree_util::Clone(base, expr->Param()->Left());

	expr = std::make_unique<Mul>(std::move(expr->Param()), std::make_unique<Ln>(std::move(base)));

	// FIXME: Handle case D(-a^x)
}

// d/dx(k) --> 0
void SetToZero(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative())
		return;

	bool is_zero = true;

	CanDifferentiate(expr->Param(), expr->RespectTo(), is_zero);

	if (is_zero)
		expr = std::make_unique<Integer>(0);

}

void SetToOne(std::unique_ptr<Expr>& expr)
{
	(void)expr;
}

void CanDifferentiate(const std::unique_ptr<Expr>& expr, std::string respect_to, bool& is_constant)
{
	if (!is_constant)
		return;

	if (expr->IsFunc())
		CanDifferentiate(expr->Param(), respect_to, is_constant);
	else if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			CanDifferentiate(expr->ChildAt(i), respect_to, is_constant);
	}
	else
	{
		if (expr->HasLeftChild())
			CanDifferentiate(expr->Left(), respect_to, is_constant);
		if (expr->HasRightChild())
			CanDifferentiate(expr->Right(), respect_to, is_constant);
	}

	if (respect_to == expr->Name())
		is_constant = false;
}

// d/dx(a1+a2+...+an) --> d/dx(a1)+d/dx(a2)+...+d/dx(an)
void DifferentiateSum(std::unique_ptr<Expr>& expr)
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

		for (int i = 0; i < expr->ChildrenSize(); i++)
			ApplyDerivativeRules(expr->ChildAt(i));
	}
	else
	{
		std::unique_ptr<Expr> left = std::make_unique<Derivative>(std::move(expr->Param()->Left()), "x");
		std::unique_ptr<Expr> right = std::make_unique<Derivative>(std::move(expr->Param()->Right()), "x");
		expr = std::make_unique<Add>(std::move(left), std::move(right));

		ApplyDerivativeRules(expr->Left());
		ApplyDerivativeRules(expr->Right());
	}
}

// d/dx(a1a2) --> d/dx(a1)a2+d/dx(a2)a1
void ProductRule(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative())
		return;

	if (!expr->Param()->IsMul())
		return;

	if (expr->Param()->IsGeneric())
	{
		std::unique_ptr<Expr> add_node = std::make_unique<Add>();
		int children_size = expr->Param()->ChildrenSize();

		for (int i = 0; i < children_size; i++)
		{
			std::unique_ptr<Expr> mul_node = std::make_unique<Mul>();
			std::unique_ptr<Expr> derivative_child;
			tree_util::Clone(derivative_child, expr->Param()->ChildAt(i));

			for (int j = 0; j < children_size; j++)
			{
				if (i != j)
				{
					std::unique_ptr<Expr> other_child;
					tree_util::Clone(other_child, expr->Param()->ChildAt(j));
					mul_node->AddChild(std::move(other_child));
				}
				else
					mul_node->AddChild(std::make_unique<Derivative>(std::move(derivative_child), "x"));
			}

			for (int k = 0; k < mul_node->ChildrenSize(); k++)
				ApplyDerivativeRules(mul_node->ChildAt(k));

			add_node->AddChild(std::move(mul_node));
		}
		
		expr = std::move(add_node);
	}
	else
	{
		std::unique_ptr<Expr> copy_left, copy_right;
		tree_util::Clone(copy_left, expr->Param()->Left());
		tree_util::Clone(copy_right, expr->Param()->Right());

		std::unique_ptr<Expr> left = std::make_unique<Mul>(std::make_unique<Derivative>(std::move(expr->Param()->Left()), "x"), std::move(copy_right));
		std::unique_ptr<Expr> right = std::make_unique<Mul>(std::make_unique<Derivative>(std::move(expr->Param()->Right()), "x"), std::move(copy_left));

		ApplyDerivativeRules(left->Left());
		ApplyDerivativeRules(right->Left());

		expr = std::make_unique<Add>(std::move(left), std::move(right));
	}
}

// d/dx(a1/a2) --> (d/dx(a1)a2-d/dx(a2)a1)/(a2^2)
void QuotientRule(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative())
		return;

	if (!expr->Param()->IsMul())
		return;

	if (expr->Param()->IsGeneric())
		return;

	if (!expr->Param()->Right()->IsPow())
		return;

	if (!expr->Param()->Right()->Right()->IsNegOne())
		return;

	std::unique_ptr<Expr> copy_left, copy_right_a, copy_right_b, numerator, denominator;

	tree_util::Clone(copy_left, expr->Param()->Left());
	tree_util::Clone(copy_right_a, expr->Param()->Right()->Left());
	tree_util::Clone(copy_right_b, expr->Param()->Right()->Left());

	std::unique_ptr<Expr> left = std::make_unique<Mul>(std::make_unique<Derivative>(std::move(expr->Param()->Left()), "x"), std::move(copy_right_a));
	std::unique_ptr<Expr> right = std::make_unique<Mul>(std::make_unique<Derivative>(std::move(expr->Param()->Right()->Left()), "x"), std::move(copy_left));

	ApplyDerivativeRules(left->Left());
	ApplyDerivativeRules(right->Left());

	numerator = std::make_unique<Add>(std::move(left), std::make_unique<Mul>(std::make_unique<Integer>(-1), std::move(right)));
	denominator = std::make_unique<Pow>(std::move(copy_right_b), std::make_unique<Integer>(-2));
	expr = std::make_unique<Mul>(std::move(numerator), std::move(denominator));
}

void ChainRule(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsDerivative())
		return;

	if (!expr->Param()->IsFunc())
		return;

	std::unique_ptr<Expr> mul_node{ std::make_unique<Mul>() };
	ApplyChainRule(expr->Param(), mul_node, true);

	if (mul_node->ChildrenSize() != 0)
	{
		mul_node->ReverseChildren();
		expr = std::move(mul_node);
	}
}

void ApplyChainRule(std::unique_ptr<Expr>& expr, std::unique_ptr<Expr>& mul_node, bool is_outermost)
{
	if (CanApplyChainRule(expr->Param()))
	{
		std::unique_ptr<Expr> copy_param;
		tree_util::Clone(copy_param, expr->Param());
		ApplyChainRule(copy_param, mul_node, false);
	}

	if (expr->Param()->IsFunc())
	{
		if (is_outermost && mul_node->ChildrenSize() != 0)
		{
			std::unique_ptr<Expr> a;
			tree_util::Clone(a, expr);
			a = std::make_unique<Derivative>(std::move(a), "x");

			ApplyDerivativeRules(a, true);
			mul_node->AddChild(std::move(a));
		}
		else
		{
			std::unique_ptr<Expr> a, b;
			tree_util::Clone(a, expr);
			tree_util::Clone(b, expr->Param());

			a = std::make_unique<Derivative>(std::move(a), "x");
			b = std::make_unique<Derivative>(std::move(b), "x");

			ApplyDerivativeRules(a, true);
			ApplyDerivativeRules(b, true);

			mul_node->AddChild(std::move(b));
			mul_node->AddChild(std::move(a));
		}
	}
}

void ApplyDerivativeRules(std::unique_ptr<Expr>& expr, bool skip_chain_rule)
{
	if (!expr->IsDerivative())
		return;

	SetToZero(expr); 

	if (expr->IsZero())
		return;

	if (!skip_chain_rule && CanApplyChainRule(expr->Param()))
		ChainRule(expr);
	else if (expr->Param()->IsAdd())
		DifferentiateSum(expr);
	else if (expr->Param()->IsMul())
	{
		if (expr->Param()->IsGeneric())
			ProductRule(expr);
		else
		{
			QuotientRule(expr);
			ProductRule(expr);
		}
	}
	else if (expr->Param()->IsPow())
	{
		if (!IsConstant(expr->Param()->Left(), expr->RespectTo()))
			PowerRule(expr);
		else if (!IsConstant(expr->Param()->Right(), expr->RespectTo()))
			ExponentialRule(expr);
	}
	else if (expr->Param()->IsFunc())
	{
		if (expr->Param()->IsSin())
			expr = std::make_unique<Cos>(std::move(expr->Param()->Param()));
		else if (expr->Param()->IsCos())
			expr = std::make_unique<Mul>(std::make_unique<Integer>(-1), std::make_unique<Sin>(std::move(expr->Param()->Param())));
		else if (expr->Param()->IsTan())
			expr = std::make_unique<Pow>(std::make_unique<Cos>(std::move(expr->Param()->Param())), std::make_unique<Integer>(-2));
		else if (expr->Param()->IsLn())
			expr = std::make_unique<Pow>(std::move(expr->Param()->Param()), std::make_unique<Integer>(-1));
		else if (expr->Param()->IsLog())
			expr = std::make_unique<Pow>(std::make_unique<Mul>(std::move(expr->Param()->Param()), std::make_unique<Ln>(std::move(expr->Param()->Base()))), std::make_unique<Integer>(-1));
	}
}

bool IsConstant(const std::unique_ptr<Expr>& expr, std::string respect_to)
{
	bool is_constant = true;
	CanDifferentiate(expr, respect_to, is_constant);

	return is_constant;
}

bool CanApplyChainRule(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsFunc())
		return false;

	if (!expr->Param()->IsFunc())
		return false;

	return true;
}

void Integrate(std::unique_ptr<Expr>& expr)
{
	(void)expr;
	// Apply integration rules
}

} // namespace calculus
