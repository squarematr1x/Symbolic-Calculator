#include "PowerTransformation.h"
#include "TreeUtil.h"
#include "Calculator.h"

#include <queue>

namespace algebra {

void ApplyExponentRules(std::unique_ptr<Expr>& root)
{
	// (ab)^n --> (a^n)(b^n)
	ExponentRuleParenthesis(root);
	// a^n^m --> a^(nm)
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
			std::unique_ptr<Expr> exponent;
			exponent = std::move(calc::AddNumbers(root->Left()->Right(), root->Right()->Right()));
			root = std::move(std::make_unique<Pow>(std::make_unique<Var>(var), std::move(exponent)));
		}
	}
}

void ApplyExponentRuleMulGenNode(std::unique_ptr<Expr>& root)
{
	std::queue<std::unique_ptr<Expr>> new_children;
	std::unique_ptr<Expr> exponent = nullptr;
	bool modified = false;

	for (int i = 0; i != root->ChildrenSize(); i++)
	{
		if (i + 1 < root->ChildrenSize())
		{
			if (PowWithNumberExponents(root->ChildAt(i), root->ChildAt(i + 1)))
			{
				if (root->ChildAt(i)->Left() == root->ChildAt(i + 1)->Left())
				{
					if (!exponent)
					{
						modified = true;
						exponent = std::move(root->ChildAt(i)->Right());
					}

					exponent = std::move(calc::AddNumbers(exponent, root->ChildAt(i + 1)->Right()));
				}
			}

			if (!modified)
				new_children.push(std::move(root->ChildAt(i)));
			else if (modified && root->ChildAt(i)->Left() != root->ChildAt(i + 1)->Left())
			{
				if (!exponent->IsZero())
				{
					std::unique_ptr<Expr> copy_exponent;
					tree_util::Clone(copy_exponent, exponent);
					new_children.push(std::make_unique<Pow>(move(root->ChildAt(i)->Left()), std::move(copy_exponent)));
					exponent = nullptr;
				}

				modified = false;
			}

		}
		else if (modified && !exponent->IsZero())
			new_children.push(std::make_unique<Pow>(move(root->ChildAt(i)->Left()), std::move(exponent)));
		else if (!modified)
			new_children.push(std::move(root->ChildAt(i)));
	}

	if (new_children.size() > 1)
	{
		root->ClearChildren();
		tree_util::MoveQueueToGenericNode(root, new_children);
		root->SortChildren();
	}
	else if (new_children.size() != 0)
		root = std::move(new_children.front());
}

// a^n^m --> a^(nm)
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

	if (root->IsPow() && root->Left()->IsPow() && root->Right()->IsNumber())
	{
		std::string value = "";

		if (root->Left()->HasLeftChild())
		{
			if (root->Left()->Left()->IsVar())
				value = root->Left()->Left()->Name();
		}

		if (root->Left()->HasRightChild())
		{
			if (root->Left()->Right()->IsNumber() && value != "")
			{
				std::unique_ptr<Expr> exponent = std::move(calc::MulNumbers(root->Right(), root->Left()->Right()));
				root = std::move(std::make_unique<Pow>(std::make_unique<Var>(value), std::move(exponent)));
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
		else
			HandleExponentRuleParenthesis(root, root->Right(), false);
	}
}

void HandleExponentRuleParenthesis(std::unique_ptr<Expr>& base, std::unique_ptr<Expr>& exponent, bool generic)
{
	std::unique_ptr<Expr> new_root;
	std::unique_ptr<Expr> exponent_copy;
	tree_util::Clone(exponent_copy, exponent);

	if (generic)
		new_root = std::make_unique<Pow>(std::move(base), std::move(exponent_copy));
	else
	{
		new_root = std::move(base->Left());

		new_root->SetLeft(std::make_unique<Pow>(std::move(new_root->Left()), std::move(exponent)));
		new_root->SetRight(std::make_unique<Pow>(std::move(new_root->Right()), std::move(exponent_copy)));
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
