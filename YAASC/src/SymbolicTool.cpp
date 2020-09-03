#include "SymbolicTool.h"

namespace yaasc {

void Simplify(std::unique_ptr<Expr>& root) 
{
	std::unique_ptr<Expr> copy;
	int i = 0;

	if (!root) // Expression might be empty
		return;

	while (true)
	{
		tree_util::DeepCopy(copy, root);
		Canonize(root);
		algebra::PowerOfSum(root);
		algebra::Expand(root);
		algebra::ApplyLogarithmRules(root);
		AddVariables(root);
		SimplifyExponents(root, false);
		ApplyExponentRules(root);
		calc::Calculate(root);
		ReduceToZero(root);
		ReduceToOne(root);
		RemoveAdditiveZeros(root);
		RemoveMulOne(root);
		Flatten(root);
		i++;

		// When simplification is done
		if (copy == root)
		{
			std::cout << "\t total iterations: " << i << '\n';
			break;
		}
	}

	// Finally simplifies variables that are raised to one: a^1 --> a
	SimplifyExponents(root, true);
}

// Adding same variables: a+2a+3a --> 6a
void AddVariables(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (!root->LeftIsTerminal())
		AddVariables(root->Left());

	if (!root->RightIsTerminal())
		AddVariables(root->Right());

	if (root->IsAdd())
	{
		if (root->IsGeneric())
			AddGenNode(root);
		else
			AddBinNode(root, root->Left(), root->Right());
	}
}

/* Addition in binary node
*
*    +         *
*   / \  -->  / \
*  a   a     2   a
*/
void AddBinNode(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& left, std::unique_ptr<Expr>& right) // FIXME: How about gen nodes (2xyz + 4yzx)
{
	int multiplier = 0;

	if (left == right)
		root = std::make_unique<Mul>(std::make_unique<Integer>(2), std::move(left));
	else if (IsMulByNumber(left) && IsMulByNumber(right))
	{
		if (left->Right() == right->Right())
		{
			multiplier += std::stoi(left->Left()->Name()) + std::stoi(right->Left()->Name());
			root = std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(left->Right()));
		}
	}
	else if (IsMulByNumber(left))
	{
		if (left->Right() == right)
		{
			multiplier += std::stoi(left->Left()->Name()) + 1;
			root = std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(right));
		}
	}
	else if (IsMulByNumber(right))
	{
		if (right->Right() == left)
		{
			multiplier += std::stoi(right->Left()->Name()) + 1;
			root = std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(left));
		}
	}
	else if (CanAddGen(left, right))
	{
		multiplier += std::stoi(left->ChildAt(0)->Name());
		multiplier += std::stoi(right->ChildAt(0)->Name());
		left->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root = std::move(left);
	}
	else if (SameGenericVariables(left, right))
	{
		multiplier += std::stoi(left->ChildAt(0)->Name()) + 1;
		left->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root = std::move(left);
	}
	else if (SameGenericVariables(right, left))
	{
		multiplier += std::stoi(right->ChildAt(0)->Name()) + 1;
		right->SetChildAt(0, std::make_unique<Integer>(multiplier));
		root = std::move(right);
	}
}

/* Addition in generic node
*
*     +          +
*   / | \       / \
*  a  b  a --> *   b
*             / \
*            2   a
*/
void AddGenNode(std::unique_ptr<Expr>& root)
{
	std::queue<std::unique_ptr<Expr>> new_children;
	int multiplier = 0;

	for (int i = 0; i != root->ChildrenSize(); i++)
	{
		if (i + 1 < root->ChildrenSize())
		{
			std::unique_ptr<Expr>& child_a = root->ChildAt(i);
			std::unique_ptr<Expr>& child_b = root->ChildAt(i + 1);

			if (child_a == child_b)
			{
				if (multiplier == 0)
					multiplier += 2;
				else
					multiplier++;
			}
			else if (IsMulByNumber(child_a) && IsMulByNumber(child_b) && child_a->Right() == child_b->Right())
			{
				if (multiplier == 0)
					multiplier += std::stoi(child_a->Left()->Name());

				multiplier += std::stoi(child_b->Left()->Name());
			}
			else if (IsMulByNumber(child_a) && child_a->Right() == child_b)
			{
				if (multiplier == 0)
					multiplier += std::stoi(child_a->Left()->Name());

				multiplier++;
			}
			else if (IsMulByNumber(child_b) && child_b->Right() == child_a)
			{
				if (multiplier == 0)
					multiplier++;

				multiplier += std::stoi(child_b->Left()->Name());
			}
			else if (CanAddGen(child_a, child_b))
			{
				if (multiplier == 0)
					multiplier += std::stoi(child_a->ChildAt(0)->Name());
				
				multiplier += std::stoi(child_b->ChildAt(0)->Name());
			}
			else if (SameGenericVariables(child_a, child_b))
			{
				if (multiplier == 0)
					multiplier += std::stoi(child_a->ChildAt(0)->Name());

				multiplier++;
			}
			else if (SameGenericVariables(child_b, child_a))
			{
				if (multiplier == 0)
					multiplier++;

				multiplier += std::stoi(child_b->ChildAt(0)->Name());
			}
			else if (multiplier != 0)
			{
				if (IsMulByNumber(child_a))
					new_children.push(std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(child_a->Right())));
				else
					new_children.push(std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(child_a)));

				multiplier = 0;
			}
			else
				new_children.push(std::move(child_a));
		}
		else
		{
			if (multiplier != 0)
			{
				if (IsGenMulByNumber(root->ChildAt(i)))
					root->ChildAt(i)->RemoveChild(0); // Old multiplier is no longer needed
					
				new_children.push(std::make_unique<Mul>(std::make_unique<Integer>(multiplier), std::move(root->ChildAt(i))));
			}
			else
				new_children.push(std::move(root->ChildAt(i)));
		}
	} 

	if (!new_children.empty())
	{
		root->ClearChildren();
		tree_util::MoveQueueToGenericNode(root, new_children);
		root->SortChildren();
	}
}

void ApplyExponentRules(std::unique_ptr<Expr>& root)
{
	// (ab)^n --> (a^n)(b^n)
	algebra::ExponentRuleParenthesis(root);
	// (a^n)^m --> a^(nm)
	algebra::ExponentRulePow(root);
	// (a^n)(a^m) --> a^(n+m)
	algebra::ExponentRuleMul(root);
}

// Simplifies variables that are raised to zero or one: a^0+a^1 --> 1+a
void SimplifyExponents(std::unique_ptr<Expr>& root, bool final_modification)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			SimplifyExponents(root->ChildAt(i), final_modification);
	}
	else if (root->IsFunc())
	{
		SimplifyExponents(root->Param(), final_modification);
	}
	else
	{
		if (!root->LeftIsTerminal())
			SimplifyExponents(root->Left(), final_modification);

		if (!root->RightIsTerminal())
			SimplifyExponents(root->Right(), final_modification);
	}

	if (RaisedToZero(root))
		root = std::move(std::make_unique<Integer>(1));

	if (final_modification)
	{
		if (RaisedToOne(root))
		{
			std::unique_ptr<Expr> left = std::move(root->Left());
			root = std::move(left);
		}

		if (root->IsGeneric())
		{
			for (int i = 0; i < root->ChildrenSize(); i++)
				SimplifyExponents(root->ChildAt(i), true);
		}
	}
}

bool Simplified(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (expr_a == expr_b)
		return true;

	return false;
}

bool CanAddGen(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
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

// This doesn't consider numbers
bool SameGenericVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
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

void Flatten(std::unique_ptr<Expr>& root)
{
	std::unique_ptr<Expr> parent = nullptr; // Root has no parent
	std::queue<std::unique_ptr<Expr>> children;
	ToGeneric(root, parent, children);
}

/* Flattens the expression tree by making it more generic:
*
*      *          *
*     / \       / | \
*    *   c --> a  b  c
*   / \
*  a   b
*
*/
void ToGeneric(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& parent, std::queue<std::unique_ptr<Expr>>& children) // FIXME: Still has issues
{
	if (root->IsTerminal())
		return;

	if (root->IsAssociative() && parent && !parent->IsTerminal())
	{
		if (root->Name() != parent->Name()) // Joint between two different operators
		{
			std::queue<std::unique_ptr<Expr>> sub_children;
			std::unique_ptr<Expr> null_parent = nullptr;
			ToGeneric(root, null_parent, sub_children);
	
			return;	// Exit sub tree
		}
	}

	if (root->IsGeneric())
	{
		int queue_size = children.size();
	
		for (int i = 0; i < root->ChildrenSize(); i++)
		{
			if (root->ChildAt(i)->IsAssociative())
				ToGeneric(root->ChildAt(i), root, children);
		}
	
		if (queue_size != static_cast<int>(children.size())) // When children size has been updated
		{
			for (int i = 0; i < root->ChildrenSize(); i++)
			{
				if (!root->ChildAt(i)->IsAssociative())
					children.push(std::move(root->ChildAt(i)));
			}
		}
	}

	//if (root->IsGeneric())
	//{
	//	std::queue<std::unique_ptr<Expr>> sub_children;
	//	std::unique_ptr<Expr> null_parent = nullptr;
	//
	//	for (int i = 0; i < root->ChildrenSize(); i++)
	//	{
	//		if (root->ChildAt(i)->IsAssociative())
	//		{
	//			if (root->IsAssociative() && root->Name() != root->ChildAt(i)->Name())
	//				ToGeneric(root->ChildAt(i), null_parent, sub_children);
	//			else
	//				ToGeneric(root->ChildAt(i), parent, children);
	//		}
	//	}
	//}
	else
	{
		if (!root->LeftIsTerminal())
			ToGeneric(root->Left(), root, children);

		if (!root->RightIsTerminal())
			ToGeneric(root->Right(), root, children);
	}

	if (root->IsAssociative())
	{
		if (parent && root->Name() == parent->Name())
		{
			if (root->IsGeneric())
			{
				for (int i = 0; i < root->ChildrenSize(); i++)
					children.push(std::move(root->ChildAt(i)));

				root->ClearChildren();
			}
			else
			{
				if (!SameExpressionTypes(root, root->Left()))
					children.push(std::move(root->Left()));

				if (!SameExpressionTypes(root, root->Right()))
					children.push(std::move(root->Right()));
			}
		}
		else if (!root->IsGeneric())
		{
			if (root->Name() != root->Left()->Name() && !children.empty())
				children.push(std::move(root->Left()));
			else if (root->Name() != root->Right()->Name() && !children.empty())
				children.push(std::move(root->Right()));
		}

		if ((!parent || parent->IsPow()) && !children.empty()) // When update is possible
		{
			if (root->IsMul())
				root = std::make_unique<Mul>();
			else
				root = std::make_unique<Add>();

			tree_util::MoveQueueToGenericNode(root, children);
			root->SortChildren();
		}
	}
}

/* Rearranges the tree, so that it's more suitable for further modifications:
*  a2 --> 2a
*  (b^2)(a^2) --> (a^2)(b^2)
*/
void Canonize(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (!root->LeftIsTerminal())
		Canonize(root->Left());

	if (!root->RightIsTerminal())
		Canonize(root->Right());

	if (!root->IsGeneric())
		CanonizeBinNode(root);
	else
	{
		CanonizeGenNode(root);
		ReduceGenNodeToBinNode(root);
	}
}

void CanonizeBinNode(std::unique_ptr<Expr>& root)
{
	if (!root->IsMul())
		return;

	if (root->Right()->IsNumber())
		root->SwapChildren();

	if (!root->Right()->IsNumber() && !root->Right()->IsAdd())
	{
		if (root->Left()->HasChildren())
		{
			if (root->Left()->IsMul())
			{
				if (root->Left()->Left()->IsNumber() && !root->Left()->Right()->IsNumber())
				{
					root = std::make_unique<Mul>(std::move(root->Left()->Left()),
					       std::make_unique<Mul>(std::move(root->Left()->Right()), std::move(root->Right())));
				}
				else if (root->Left()->Right()->IsNumber() && !root->Left()->Left()->IsNumber())
				{
					root = std::make_unique<Mul>(std::move(root->Left()->Right()),
				           std::make_unique<Mul>(std::move(root->Left()->Left()), std::move(root->Right())));
				}
			}
		}
	}

	if (root->HasPowChildren())
	{
		if (root->Left()->Left()->Name() > root->Right()->Left()->Name())
			root->SwapChildren();
	}
}

void CanonizeGenNode(std::unique_ptr<Expr>& root)
{
	if (!root->IsAdd())
		return;

	int limit = root->ChildrenSize();
	for (int i = 0; i != limit; i++)
	{
		if (root->ChildAt(i)->IsMul())
		{
			if (!root->ChildAt(i)->IsGeneric())
				CanonizeBinNode(root->ChildAt(i));
			else
				CanonizeGenNode(root->ChildAt(i));
		}
	}
		
}

void ReduceGenNodeToBinNode(std::unique_ptr<Expr>& root)
{
	if (!root->IsGeneric())
		return;

	if (root->ChildrenSize() != 2)
		return;

	if (root->IsMul())
		root = std::make_unique<Mul>(std::move(root->ChildAt(0)), std::move(root->ChildAt(1)));
	else
		root = std::make_unique<Add>(std::move(root->ChildAt(0)), std::move(root->ChildAt(1)));
}

bool RaisedToZero(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsPow())
		return false;

	if (expr->HasRightChild())
	{
		if (expr->Right()->Name() == "0")
			return true;
	}

	return false;
}

bool RaisedToOne(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsPow())
		return false;

	if (expr->HasRightChild())
	{
		if (expr->Right()->Name() == "1")
			return true;
	}

	return false;
}

void RemoveMulOne(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			RemoveMulOne(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			RemoveMulOne(root->Left());

		if (!root->RightIsTerminal())
			RemoveMulOne(root->Right());
	}

	if (!root->IsMul())
		return;

	if (!root->IsGeneric())
	{
		if (root->Left()->IsNumber() && root->HasRightChild())
		{
			if (root->Left()->IsOne())
				root = std::move(root->Right());
		}
		else if (root->Right()->IsNumber() && root->HasLeftChild())
		{
			if (root->Right()->IsOne())
				root = std::move(root->Left());
		}
	}
	else
	{
		std::unique_ptr<Expr> new_mul_node = std::make_unique<Mul>();

		for (int i = 0; i < root->ChildrenSize(); i++)
		{
			if (!root->ChildAt(i)->IsOne())
				new_mul_node->AddChild(std::move(root->ChildAt(i)));
		}

		if (new_mul_node->ChildrenSize() != 0)
			root = std::move(new_mul_node);
	}
}

void RemoveAdditiveZeros(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (!root->LeftIsTerminal())
		RemoveAdditiveZeros(root->Left());

	if (!root->RightIsTerminal())
		RemoveAdditiveZeros(root->Right());

	if (root->IsAdd())
	{		
		if (!root->IsGeneric())
		{
			if (root->Right()->IsZero() && !root->Left()->IsZero())
				root = std::move(root->Left());
			else if (root->Left()->IsZero() && !root->Right()->IsZero())
				root = std::move(root->Right());
		}
		else
		{
			int index_zero = 0;
			for (int i = 0; i < root->ChildrenSize(); i++)
				if (root->ChildAt(i)->IsZero())
					index_zero = i;
			
			if (index_zero != 0)
				root->RemoveChild(index_zero);
		}
	}
}

void ReduceToZero(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			ReduceToZero(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			ReduceToZero(root->Left());

		if (!root->RightIsTerminal())
			ReduceToZero(root->Right());
	}

	if (root->IsMul())
	{
		if (!root->IsGeneric())
		{
			if (root->Left()->IsZero() || root->Right()->IsZero())
				root = std::make_unique<Integer>(0);
		}
		else
		{
			bool is_zero = false;

			for (int i = 0; i < root->ChildrenSize(); i++)
			{
				if (root->ChildAt(i)->IsZero())
				{
					is_zero = true;
					break;
				}
			}

			if (is_zero)
				root = std::make_unique<Integer>(0);
		}
	}
	else if (root->IsPow())
	{
		if (root->Left()->IsZero())
			root = std::make_unique<Integer>(0);
	}
}

void ReduceToOne(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			ReduceToOne(root->ChildAt(i));
	}
	else
	{
		if (!root->LeftIsTerminal())
			ReduceToOne(root->Left());

		if (!root->RightIsTerminal())
			ReduceToOne(root->Right());
	}

	if (root->IsPow())
	{
		if (root->Left()->IsOne())
			root = std::make_unique<Integer>(1);
	}
}

bool SameExpressionTypes(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (!expr_a)
		return false;

	if (!expr_b)
		return false;

	if (expr_a->ExpressionType() == expr_b->ExpressionType())
		return true;

	return false;
}

bool IsMulByNumber(const std::unique_ptr<Expr>& expr)
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

bool IsGenMulByNumber(const std::unique_ptr<Expr>& expr)
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

} // namespace yaasc
