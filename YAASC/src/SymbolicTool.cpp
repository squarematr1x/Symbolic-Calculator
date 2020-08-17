#include "SymbolicTool.h"

namespace yaasc {

void Simplify(std::unique_ptr<Expr>& root) 
{
	std::unique_ptr<Expr> copy;
	int i = 0;

	while (true)
	{
		tree_util::DeepCopy(copy, root);
		Canonize(root);
		algebra::PowerOfSum(root);
		MultiplySum(root);
		AddVariables(root);
		ApplyExponentRules(root);
		SimplifyExponents(root, false);
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

// Multiplication of a sum: a(b+c+d) --> ab+ac+ad
void MultiplySum(std::unique_ptr<Expr>& root)
{
	if (root->HasNoChildren() && !root->IsGeneric())
		return;

	if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			MultiplySum(root->ChildAt(i));
	}
	else
	{
		if (!IsTerminal(root->Left()))
			MultiplySum(root->Left());

		if (!IsTerminal(root->Right()))
			MultiplySum(root->Right());
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

		for (int j = 0; j < root->ChildrenSize(); j++)
		{
			tree_util::DeepCopy(copy, root->ChildAt(j));
			new_left->AddChild(std::move(copy));
			new_right->AddChild(std::move(root->ChildAt(j)));
		}

		new_left->AddChild(std::move(add_child->Left()));
		new_right->AddChild(std::move(add_child->Right()));
		add_child = std::make_unique<Add>(std::move(new_left), std::move(new_right));
	}

	root = std::move(add_child);
}

// Adding same variables: a+2a+3a --> 6a
void AddVariables(std::unique_ptr<Expr>& root)
{
	if (root->HasNoChildren() && !root->IsGeneric())
		return;

	if (!IsTerminal(root->Left()))
		AddVariables(root->Left());

	if (!IsTerminal(root->Right()))
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

void PowerOfSum(std::unique_ptr<Expr>& root)
{
	// Implement power of sums here
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
	if (root->HasNoChildren() && !root->IsGeneric())
		return;

	if (!IsTerminal(root->Left()))
		SimplifyExponents(root->Left(), final_modification);

	if (!IsTerminal(root->Right()))
		SimplifyExponents(root->Right(), final_modification);

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

bool CanMultiplyBinSumNode(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsAdd())
		return false;

	if (expr->IsGeneric())
		return false;

	if (!(IsTerminal(expr->Left()) || expr->Left()->IsPow() || expr->Left()->IsMul()))
		return false;

	if (!(IsTerminal(expr->Right()) || expr->Right()->IsPow() || expr->Left()->IsMul()))
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
void ToGeneric(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& parent, std::queue<std::unique_ptr<Expr>>& children)
{
	if (root->HasNoChildren() && !root->IsGeneric())
		return;

	if (root->IsAssociative() && !IsNull(parent) && !IsTerminal(parent))
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
		
		if (queue_size != children.size()) // When children size has been updated
		{
			for (int i = 0; i < root->ChildrenSize(); i++)
			{
				if (!root->ChildAt(i)->IsAssociative())
					children.push(std::move(root->ChildAt(i)));
			}
		}
	}
	else
	{
		if (!IsTerminal(root->Left()))
			ToGeneric(root->Left(), root, children);

		if (!IsTerminal(root->Right()))
			ToGeneric(root->Right(), root, children);
	}

	if (root->IsAssociative())
	{
		if (!IsNull(parent) && root->Name() == parent->Name())
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

		if ((IsNull(parent) || parent->IsPow()) && !children.empty()) // When at root
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
	if (root->HasNoChildren() && !root->IsGeneric())
		return;

	if (!IsTerminal(root->Left()))
		Canonize(root->Left());

	if (!IsTerminal(root->Right()))
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
	if (root->HasNoChildren())
		return;

	if (!IsTerminal(root->Left()))
		RemoveMulOne(root->Left());

	if (!IsTerminal(root->Right()))
		RemoveMulOne(root->Right());

	if (!root->IsMul())
		return;

	if (!root->IsGeneric())
	{
		if (root->Left()->IsNumber() && root->HasRightChild())
		{
			if (root->Left()->Name() == "1")
				root = std::move(root->Right());
		}
		else if (root->Right()->IsNumber() && root->HasLeftChild())
		{
			if (root->Right()->Name() == "1")
				root = std::move(root->Left());
		}
	}
}

void RemoveAdditiveZeros(std::unique_ptr<Expr>& root)
{
	if (root->HasNoChildren() && !root->IsGeneric())
		return;

	if (!IsTerminal(root->Left()))
		RemoveAdditiveZeros(root->Left());

	if (!IsTerminal(root->Right()))
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
	if (root->HasNoChildren())
		return;

	if (!IsTerminal(root->Left()))
		ReduceToZero(root->Left());

	if (!IsTerminal(root->Right()))
		ReduceToZero(root->Right());

	if (root->IsMul())
	{
		if (root->Left()->IsZero() || root->Right()->IsZero())
			root = std::make_unique<Integer>(0);
	}
	else if (root->IsPow())
	{
		if (root->Left()->IsZero())
			root = std::make_unique<Integer>(0);
	}
}

void ReduceToOne(std::unique_ptr<Expr>& root)
{
	if (root->HasNoChildren())
		return;

	if (!IsTerminal(root->Left()))
		ReduceToOne(root->Left());

	if (!IsTerminal(root->Right()))
		ReduceToOne(root->Right());

	if (root->IsPow())
	{
		if (root->Left()->IsOne())
			root = std::make_unique<Integer>(1);
	}
}

bool IsTerminal(const std::unique_ptr<Expr>& expr)
{
	if (IsNull(expr))
		return true;

	if (expr->IsVar())
		return true;

	if (expr->IsNumber())
		return true;

	return false;
}

bool SameExpressionTypes(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	if (IsNull(expr_a))
		return false;

	if (IsNull(expr_b))
		return false;

	if (expr_a->ExpressionType() == expr_b->ExpressionType())
		return true;

	return false;
}

bool IsNull(const std::unique_ptr<Expr>& expr)
{
	if (!expr)
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
