#include "Expand.h"

namespace algebra {

// Multiplication of a sum: a(b+c+d) --> ab+ac+ad
void Expand(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			Expand(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			Expand(root->Left());

		if (!root->RightIsTerminal())
			Expand(root->Right());
	}

	if (!root->IsMul())
		return;

	if (root->IsGeneric())
		MultiplyGenNode(root);
	else
		MultiplyBinNode(root);
}

void MultiplyBinNode(std::unique_ptr<Expr>& root)
{
	std::unique_ptr<Expr> copy;

	if (CanMultiplyBinSumNode(root->Right()))
	{
		tree_util::DeepCopy(copy, root->Left());
		root = std::make_unique<Add>(std::make_unique<Mul>(std::move(root->Left()), std::move(root->Right()->Left())),
			std::make_unique<Mul>(std::move(copy), std::move(root->Right()->Right())));
	}
	else if (CanMultiplyBinSumNode(root->Left()))
	{
		tree_util::DeepCopy(copy, root->Right());
		root = std::make_unique<Add>(std::make_unique<Mul>(std::move(root->Right()), std::move(root->Left()->Left())),
			std::make_unique<Mul>(std::move(copy), std::move(root->Left()->Right())));
	}
	else if (CanMultiplyGenSumNode(root->Right()))
	{
		for (int i = 0; i < root->Right()->ChildrenSize(); i++)
		{
			tree_util::DeepCopy(copy, root->Left());
			std::unique_ptr<Expr> new_child = std::make_unique<Mul>(std::move(copy), std::move(root->Right()->ChildAt(i)));
			root->Right()->SetChildAt(i, std::move(new_child));
		}
		root = std::move(root->Right());
	}
	else if (CanMultiplyGenSumNode(root->Left()))
	{
		for (int i = 0; i < root->Left()->ChildrenSize(); i++)
		{
			tree_util::DeepCopy(copy, root->Right());
			std::unique_ptr<Expr> new_child = std::make_unique<Mul>(std::move(copy), std::move(root->Left()->ChildAt(i)));
			root->Left()->SetChildAt(i, std::move(new_child));
		}
		root = std::move(root->Left());
	}
}

void MultiplyGenNode(std::unique_ptr<Expr>& root)
{
	int remove_index = 0;
	bool has_add_child = false;
	std::unique_ptr<Expr> add_child;
	std::unique_ptr<Expr> copy;

	for (int i = 0; i < root->ChildrenSize(); i++)
	{
		if (root->ChildAt(i)->IsAdd())
		{
			add_child = std::move(root->ChildAt(i));
			remove_index = i;
			has_add_child = true;
			break;
		}
	}

	if (!has_add_child)
		return;

	root->RemoveChild(remove_index);

	if (add_child->IsGeneric())
	{
		for (int i = 0; i < add_child->ChildrenSize(); i++)
		{
			std::unique_ptr<Expr> new_child = std::make_unique<Mul>();

			for (int j = 0; j < root->ChildrenSize(); j++)
			{
				tree_util::DeepCopy(copy, root->ChildAt(j));
				new_child->AddChild(std::move(copy));
			}

			new_child->AddChild(std::move(add_child->ChildAt(i)));
			add_child->SetChildAt(i, std::move(new_child));
		}
	}
	else if (add_child->HasChildren())
	{
		std::unique_ptr<Expr> new_left = std::make_unique<Mul>();
		std::unique_ptr<Expr> new_right = std::make_unique<Mul>();

		for (int i = 0; i < root->ChildrenSize(); i++)
		{
			tree_util::DeepCopy(copy, root->ChildAt(i));
			new_left->AddChild(std::move(copy));
			new_right->AddChild(std::move(root->ChildAt(i)));
		}

		new_left->AddChild(std::move(add_child->Left()));
		new_right->AddChild(std::move(add_child->Right()));
		add_child = std::make_unique<Add>(std::move(new_left), std::move(new_right));
	}

	root = std::move(add_child);
}

bool CanMultiplyBinSumNode(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsAdd())
		return false;

	if (expr->IsGeneric())
		return false;

	if (!(expr->LeftIsTerminal() || expr->Left()->IsPow() || expr->Left()->IsMul()))
		return false;

	if (!(expr->RightIsTerminal() || expr->Right()->IsPow() || expr->Left()->IsMul()))
		return false;

	return true;
}

bool CanMultiplyGenSumNode(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsGeneric())
		return false;

	if (!expr->IsAdd())
		return false;

	return true;
}

} // namespace algebra
