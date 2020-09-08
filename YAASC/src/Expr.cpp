#include "Expr.h"

bool Expr::HasLeftChild()
{
	if (m_left)
		return true;

	return false;
}

bool Expr::HasRightChild()
{
	if (m_right)
		return true;

	return false;
}

bool Expr::HasChildren()
{
	if (m_left && m_right)
		return true;

	return false;
}

bool Expr::HasNoChildren()
{
	if (!m_left && !m_right)
		return true;

	return false;
}

bool Expr::HasAssociativeChildren()
{
	if (HasNoChildren())
		return false;

	if (Left()->IsAssociative())
		return true;

	if (Right()->IsAssociative())
		return true;

	return false;
}

bool Expr::HasNumberChildren()
{
	if (HasNoChildren())
		return false;

	if (!Left()->IsNumber())
		return false;

	if (!Right()->IsNumber())
		return false;

	return true;
}

bool Expr::HasMulChildren()
{
	if (!Left()->IsMul())
		return false;

	if (!Right()->IsMul())
		return false;

	return true;
}

bool Expr::HasPowChildren()
{
	if (!HasLeftChild())
		return false;

	if (!Left()->IsPow())
		return false;

	if (!HasRightChild())
		return false;

	if (!Right()->IsPow())
		return false;

	return true;
}

bool Expr::HasFloatChild()
{
	if (HasNoChildren())
		return false;

	if (Left()->IsFloat())
		return true;

	if (Right()->IsFloat())
		return true;

	return false;
}

bool Expr::LeftIsTerminal()
{
	if (!HasLeftChild())
		return true;

	if (Left()->IsTerminal())
		return true;

	return false;
}

bool Expr::RightIsTerminal()
{
	if (!HasRightChild())
		return true;

	if (Right()->IsTerminal())
		return true;

	return false;
}

void Expr::SwapChildren()
{
	std::unique_ptr<Expr> new_right = std::move(m_left);
	m_left = std::move(m_right);
	m_right = std::move(new_right);
}

void Associative::SetChildAt(int i, std::unique_ptr<Expr> child)
{
	if (i < ChildrenSize())
		m_children[i] = std::move(child);
}

void Associative::SortChildren()
{
	if (IsGeneric())
	{
		for (int i = 0; i < ChildrenSize(); i++)
			ChildAt(i)->SortChildren();
	}

	std::sort(m_children.begin(), m_children.end(),
		[](std::unique_ptr<Expr> const& a, std::unique_ptr<Expr> const& b) {
			return LeftmostChild(a)->Name() < LeftmostChild(b)->Name();
		});
}

void Associative::SortMulChildren()
{
	// Implement sorting of multiplication node
}

void Associative::SortAddChildren()
{
	// Implement sorting of add node
}

void Associative::RemoveChild(int i)
{
	if (i < ChildrenSize())
		m_children.erase(m_children.begin() + i);
}

void Associative::RemoveChildren(int from, int to)
{
	if (to <= ChildrenSize())
		m_children.erase(m_children.begin() + from, m_children.begin() + to);
}

bool Associative::IsGeneric()
{
	if (!m_children.empty())
		return true;

	return false;
}

std::unique_ptr<Expr>& Associative::ChildAt(int i)
{
	if (i < ChildrenSize())
		return m_children[i];

	return Left();
}

bool Integer::IsZero()
{
	if (Name() == "0")
		return true;

	return false;
}

bool Integer::IsOne()
{
	if (Name() == "1")
		return true;

	return false;
}

bool Integer::IsNegOne()
{
	if (Name() == "-1")
		return true;

	return false;
}

bool Float::IsZero()
{
	if (Name() == "0")
		return true;

	return false;

}

bool Float::IsOne()
{
	if (Name() == "1")
		return true;

	return false;
}

bool Float::IsNegOne()
{
	if (Name() == "-1")
		return true;

	return false;
}

std::string Float::Name() const // FIXME: This could probably be simplified
{
	std::string float_str = std::to_string(m_atom);
	std::string new_str = "";

	int end = float_str.length();
	bool isZero = true;

	for (unsigned int i = 0; i < float_str.length(); i++)
	{
		if (float_str[i] != '0' && float_str[i] != '.')
		{
			isZero = false;
			end = i;
		}
	}

	if (!isZero)
	{
		for (int i = 0; i <= end; i++)
			new_str += float_str[i];
	}
	else
		new_str += "0";

	return new_str;
}

std::ostream& operator<< (std::ostream& out, const std::unique_ptr<Expr>& expr)
{
	out << expr->Name();
	return out;
}

bool operator== (const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	return SameExpressions(expr_a, expr_b);
}

bool operator!= (const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	return !SameExpressions(expr_a, expr_b);
}

bool SameExpressions(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b)
{
	bool same_expressions = true;
	CheckExpressions(expr_a, expr_b, same_expressions);

	return same_expressions;
}

void CheckExpressions(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b, bool& same)
{
	if (!expr_a || !expr_b)
		same = false;

	if (!same)
		return;

	if (expr_a->HasLeftChild() && expr_b->HasLeftChild())
		CheckExpressions(expr_a->Left(), expr_b->Left(), same);

	if (expr_a->HasRightChild() && expr_b->HasRightChild())
		CheckExpressions(expr_a->Right(), expr_b->Right(), same);

	if (expr_a->IsFunc() && expr_b->IsFunc())
		CheckExpressions(expr_a->Param(), expr_b->Param(), same);

	if (expr_a->IsGeneric() && expr_b->IsGeneric())
	{
		if (expr_a->ChildrenSize() != expr_b->ChildrenSize())
			same = false;
		else
		{
			for (int i = 0; i < expr_a->ChildrenSize(); i++)
				CheckExpressions(expr_a->ChildAt(i), expr_b->ChildAt(i), same);
		}
	}
	else if (!expr_a->IsGeneric() && expr_b->IsGeneric()) // FIXME: Generic and binary nodes could be the same in some cases
		same = false;
	else if (expr_a->IsGeneric() && !expr_b->IsGeneric()) // FIXME: Generic and binary nodes could be the same in some cases
		same = false;

	if (expr_a->Name() != expr_b->Name())
		same = false;
}

const std::unique_ptr<Expr>& LeftmostChild(const std::unique_ptr<Expr>& expr)
{
	if (expr->HasLeftChild())
		return LeftmostChild(expr->Left());
	else if (expr->IsGeneric())
		return LeftmostChild(expr->ChildAt(0));

	return expr;
}

const std::unique_ptr<Expr>& LeftmostAssociativeOperator(const std::unique_ptr<Expr>& expr)
{
	if (expr->HasLeftChild() && expr->Left()->IsAssociative())
		return LeftmostAssociativeOperator(expr->Left());
	else if (expr->IsGeneric() && expr->ChildrenSize() != 0 && expr->ChildAt(0)->IsAssociative())
		return LeftmostAssociativeOperator(expr->ChildAt(0));

	return expr;
}
