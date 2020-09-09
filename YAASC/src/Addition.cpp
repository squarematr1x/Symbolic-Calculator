#include "Addition.h"

namespace algebra {

void AddVariables(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			AddVariables(expr->ChildAt(i));
	}
	else
	{
		if (!expr->LeftIsTerminal())
			AddVariables(expr->Left());

		if (!expr->RightIsTerminal())
			AddVariables(expr->Right());
	}

	if (!expr->IsAdd())
		return;

	if (expr->IsGeneric())
		AddGenNode(expr);
	else
		AddBinNodes(expr, expr->Left(), expr->Right());
}

/* Addition in binary node
*
*    +         *
*   / \  -->  / \
*  a   a     2   a
*/
void AddBinNodes(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& left, std::unique_ptr<Expr>& right)
{
	int multiplier = 0;

	if (left == right)
		root = std::make_unique<Mul>(std::make_unique<Integer>(2), std::move(left));
	else if (IsMultipliedByNumber(left) && IsMultipliedByNumber(right))
	{
		if (left->Right() == right->Right())
		{
			multiplier += std::stoi(left->Left()->Name()) + std::stoi(right->Left()->Name());
			root = std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(left->Right()));
		}
	}
	else if (IsMultipliedByNumber(left))
	{
		if (left->Right() == right)
		{
			multiplier += std::stoi(left->Left()->Name()) + 1;
			root = std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(right));
		}
	}
	else if (IsMultipliedByNumber(right))
	{
		if (right->Right() == left)
		{
			multiplier += std::stoi(right->Left()->Name()) + 1;
			root = std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(left));
		}
	}
	else if (CanAddGenNode(left, right))
	{
		multiplier += std::stoi(left->ChildAt(0)->Name()) + std::stoi(right->ChildAt(0)->Name());
		left->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root = std::move(left);
	}
	else if (SameGenVariables(left, right))
	{
		multiplier += std::stoi(left->ChildAt(0)->Name()) + 1;
		left->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root = std::move(left);
	}
	else if (SameGenVariables(right, left))
	{
		multiplier += std::stoi(right->ChildAt(0)->Name()) + 1;
		right->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root = std::move(right);
	}
}

// a1+a2+...+an
void AddGenNode(std::unique_ptr<Expr>& expr)
{
	std::unique_ptr<Expr> add_node = std::make_unique<Add>();
	int children_size = expr->ChildrenSize();

	for (int i = 0; i < children_size; i++)
	{
		if (expr->ChildAt(i)->IsZero())
		{
			++i;

			if (i >= children_size)
				break;
		}

		if (i + 1 < children_size)
		{
			for (int j = i + 1; j < children_size; j++)
			{
				std::unique_ptr<Expr> old;
				tree_util::DeepCopy(old, expr->ChildAt(i));
				AddGenNodes(expr, i, expr->ChildAt(i), expr->ChildAt(j));

				if (old != expr->ChildAt(i))
					expr->SetChildAt(j, std::make_unique<Integer>(0));
			}
		}

		add_node->AddChild(std::move(expr->ChildAt(i)));
	}

	if (add_node->ChildrenSize() > 0)
		expr = std::move(add_node);
}

/* Addition in generic node
*
*     +          +
*   / | \       / \
*  a  b  a --> *   b
*             / \
*            2   a
*/
void AddGenNodes(std::unique_ptr<Expr>& root, int index, std::unique_ptr<Expr>& child_a, std::unique_ptr<Expr>& child_b)
{
	int multiplier = 0;

	if (child_a == child_b)
		root->SetChildAt(index, std::make_unique<Mul>(std::make_unique<Integer>(2), std::move(child_a)));
	else if (IsMultipliedByNumber(child_a) && IsMultipliedByNumber(child_b))
	{
		if (child_a->Right() == child_b->Right())
		{
			multiplier += std::stoi(child_a->Left()->Name()) + std::stoi(child_b->Left()->Name());
			root->SetChildAt(index, std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(child_a->Right())));
		}
	}
	else if (IsMultipliedByNumber(child_a))
	{
		if (child_a->Right() == child_b)
		{
			multiplier += std::stoi(child_a->Left()->Name()) + 1;
			root->SetChildAt(index, std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(child_b)));
		}
	}
	else if (IsMultipliedByNumber(child_b))
	{
		if (child_b->Right() == child_a)
		{
			multiplier += std::stoi(child_b->Left()->Name()) + 1;
			root->SetChildAt(index, std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(child_a)));
		}
	}
	else if (CanAddGenNode(child_a, child_b))
	{
		multiplier += std::stoi(child_a->ChildAt(0)->Name()) + std::stoi(child_b->ChildAt(0)->Name());
		child_a->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root->SetChildAt(index, std::move(child_a));
	}
	else if (SameGenVariables(child_a, child_b))
	{
		multiplier += std::stoi(child_a->ChildAt(0)->Name()) + 1;
		child_a->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root->SetChildAt(index, std::move(child_a));
	}
	else if (SameGenVariables(child_b, child_a))
	{
		multiplier += std::stoi(child_b->ChildAt(0)->Name()) + 1;
		child_b->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root->SetChildAt(index, std::move(child_b));
	}
}

bool IsMultipliedByNumber(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsMul())
		return false;

	if (expr->HasNoChildren())
		return false;

	if (!expr->Left()->IsNumber())
		return false;

	if (expr->Right()->IsNumber())
		return false;

	return true;
}

bool IsGenMultipliedByNumber(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsMul())
		return false;

	if (!expr->IsGeneric())
		return false;

	if (expr->ChildrenSize() == 0)
		return false;

	if (!expr->ChildAt(0)->IsNumber())
		return false;

	return true;
}

bool CanAddGenNode(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (!expr_a->IsGeneric())
		return false;

	if (!expr_b->IsGeneric())
		return false;

	if (expr_a->ChildrenSize() != expr_b->ChildrenSize())
		return false;

	if (expr_a->ChildrenSize() < 2)
		return false;

	if (!expr_a->ChildAt(0)->IsNumber())
		return false;

	if (!expr_b->ChildAt(0)->IsNumber())
		return false;

	bool same = true;

	for (int i = 1; i < expr_a->ChildrenSize(); i++)
	{
		if (expr_a->ChildAt(i) != expr_b->ChildAt(i))
		{
			same = false;
			break;
		}
	}

	return same;
}

bool SameGenVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (!expr_a->IsGeneric())
		return false;

	if (!expr_b->IsGeneric())
		return false;

	if (expr_a->ChildrenSize() != expr_b->ChildrenSize() + 1)
		return false;

	bool same = true;

	for (int i = 1; i < expr_a->ChildrenSize(); i++)
	{
		if (expr_a->ChildAt(i) != expr_b->ChildAt(i - 1))
		{
			same = false;
			break;
		}
	}

	return same;
}

}
