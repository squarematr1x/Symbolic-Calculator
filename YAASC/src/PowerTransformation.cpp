#include "PowerTransformation.h"

namespace algebra {

void ApplyExponentRules(std::unique_ptr<Expr>& root)
{
	// (ab)^n --> (a^n)(b^n)
	ExponentRuleParenthesis(root);
	// (a^n)^m --> a^(nm)
	ExponentRulePow(root);
	// (a^n)(a^m) --> a^(n+m)
	ExponentRuleMul(root);
}

// (a^n)(a^m) --> a^(n+m)
void ExponentRuleMul(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			ExponentRuleMul(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			ExponentRuleMul(root->Left());

		if (!root->RightIsTerminal())
			ExponentRuleMul(root->Right());
	}

	if (!root->IsMul())
		return;

	if (root->IsGeneric())
		ApplyExponentRuleMulGenNode(root);
	else if (root->HasChildren() && root->Left()->IsPow() && root->Right()->IsPow())
		ApplyExponentRuleMulBinNode(root);
}

void ApplyExponentRuleMulBinNode(std::unique_ptr<Expr>& root)
{
	if (SameVariables(root->Left()->Left(), root->Right()->Left()))
	{
		std::string var = root->Left()->Left()->Name();
		if (CanApplyExponentRule(root, var))
		{
			float exponent = root->Left()->Right()->fValue() + root->Right()->Right()->fValue();

			if (abs(exponent - floor(exponent)) < 0.000001f)
				root = std::move(std::make_unique<Pow>(std::make_unique<Var>(var), std::make_unique<Float>(exponent)));
			else
				root = std::move(std::make_unique<Pow>(std::make_unique<Var>(var), std::make_unique<Integer>(static_cast<int>(exponent))));
		}
	}
}

void ApplyExponentRuleMulGenNode(std::unique_ptr<Expr>& root)
{
	std::queue<std::unique_ptr<Expr>> new_children;
	int exponent = 0;
	bool modified = false;

	for (int i = 0; i != root->ChildrenSize(); i++)
	{
		if (i + 1 < root->ChildrenSize())
		{
			if (PowWithNumberExponents(root->ChildAt(i), root->ChildAt(i + 1)))
			{
				if (root->ChildAt(i)->Left() == root->ChildAt(i + 1)->Left())
				{
					if (exponent == 0)
					{
						modified = true;
						exponent += root->ChildAt(i)->Right()->iValue();
					}

					exponent += root->ChildAt(i + 1)->Right()->iValue();
				}
				else if (!modified)
					new_children.push(std::move(root->ChildAt(i)));
			}
			else if (!modified)
				new_children.push(std::move(root->ChildAt(i)));

			if (modified && root->ChildAt(i)->Left() != root->ChildAt(i + 1)->Left())
			{
				if (exponent != 0)
				{
					new_children.push(std::make_unique<Pow>(move(root->ChildAt(i)->Left()), std::make_unique<Integer>(exponent)));
					exponent = 0;
				}

				modified = false;
			}

		}
		else if (modified && exponent != 0)
		{
			new_children.push(std::make_unique<Pow>(move(root->ChildAt(i)->Left()), std::make_unique<Integer>(exponent)));
			modified = false;
			exponent = 0;
		}
		else if (!modified)
			new_children.push(std::move(root->ChildAt(i)));
	}

	root->ClearChildren();
	tree_util::MoveQueueToGenericNode(root, new_children);
	root->SortChildren();
}

// (a^n)^m --> a^(nm)
void ExponentRulePow(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;
	
	if (root->IsGeneric())
	{
		for (int i = 0; i != root->ChildrenSize(); i++)
			ExponentRulePow(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			ExponentRulePow(root->Left());

		if (!root->RightIsTerminal())
			ExponentRulePow(root->Right());
	}

	std::string value = "";
	if (root->IsPow() && root->Left()->IsPow() && root->Right()->IsInteger())
	{
		if (root->Left()->HasLeftChild())
		{
			if (root->Left()->Left()->IsVar())
				value = root->Left()->Left()->Name();
		}

		if (root->Left()->HasRightChild())
		{
			if (root->Left()->Right()->IsInteger() && value != "")
			{
				int exponent = root->Right()->iValue() * root->Left()->Right()->iValue();

				root = std::move(std::make_unique<Pow>(std::make_unique<Var>(value),
				       std::make_unique<Integer>(exponent)));
			}
		}
	}
}

// (ab)^n --> (a^n)(b^n)
void ExponentRuleParenthesis(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			ExponentRuleParenthesis(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			ExponentRuleParenthesis(root->Left());

		if (!root->RightIsTerminal())
			ExponentRuleParenthesis(root->Right());
	}

	if (!root->IsPow())
		return;

	if (root->Left()->IsMul() && root->RightIsTerminal())
	{
		if (root->Left()->IsGeneric())
		{
			std::queue<std::unique_ptr<Expr>> new_children;
			int length = root->Left()->ChildrenSize();
			for (int i = 0; i != length; i++)
			{
				std::unique_ptr<Expr> expr = std::move(root->Left()->ChildAt(i));
				HandleExponentRuleParenthesis(expr, root->Right(), true);
				new_children.push(std::move(expr));
			}

			root = std::make_unique<Mul>();
			tree_util::MoveQueueToGenericNode(root, new_children);
			root->SortChildren();
		}
		else if (root->Left()->HasPowChildren())
			HandleExponentRuleParenthesis(root, root->Right(), false);
	}
}

void HandleExponentRuleParenthesis(std::unique_ptr<Expr>& base, std::unique_ptr<Expr>& exponent, bool generic) // FIXME: How about (abc)^(n^1)?
{
	std::unique_ptr<Expr> new_root;

	if (generic)
	{
		if (exponent->IsInteger())
			new_root = std::make_unique<Pow>(std::move(base), std::make_unique<Integer>(exponent->iValue()));
		else if (exponent->IsFloat())
			new_root = std::make_unique<Pow>(std::move(base), std::make_unique<Float>(exponent->fValue()));
		else if (exponent->IsVar())
			new_root = std::make_unique<Pow>(std::move(base), std::make_unique<Var>(exponent->Name()));
	}
	else
	{
		new_root = std::move(base->Left());
		if (exponent->IsInteger())
		{
			new_root->SetLeft(std::make_unique<Pow>(std::move(new_root->Left()), std::make_unique<Integer>(exponent->iValue())));
			new_root->SetRight(std::make_unique<Pow>(std::move(new_root->Right()), std::make_unique<Integer>(exponent->iValue())));
		}
		else if (exponent->IsFloat())
		{
			new_root->SetLeft(std::make_unique<Pow>(std::move(new_root->Left()), std::make_unique<Float>(exponent->fValue())));
			new_root->SetRight(std::make_unique<Pow>(std::move(new_root->Right()), std::make_unique<Float>(exponent->fValue())));
		}
		else if (exponent->IsVar())
		{
			new_root->SetLeft(std::make_unique<Pow>(std::move(new_root->Left()), std::make_unique<Var>(exponent->Name())));
			new_root->SetRight(std::make_unique<Pow>(std::move(new_root->Right()), std::make_unique<Var>(exponent->Name())));
		}
	}

	base = std::move(new_root);
}


bool PowWithNumberExponents(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (!expr_a->IsPow() || !expr_b->IsPow())
		return false;

	if (!expr_a->Right()->IsNumber() || !expr_b->Right()->IsNumber())
		return false;

	return true;
}

bool CanApplyExponentRule(const std::unique_ptr<Expr>& expr, std::string value)
{
	if (expr->Left()->HasChildren())
	{
		if (expr->Left()->Left()->Name() == value && expr->Left()->Right()->IsNumber())
			return true;
	}
	else if (expr->Right()->HasChildren())
	{
		if (expr->Right()->Left()->Name() == value && expr->Right()->Right()->IsNumber())
			return true;
	}

	return false;
}

bool SameVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (!expr_a->IsVar())
		return false;

	if (!expr_b->IsVar())
		return false;

	if (expr_a->Name() == expr_b->Name())
		return true;

	return false;
}

} // namespace algebra
