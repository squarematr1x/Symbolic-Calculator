#include "Logarithm.h"

namespace algebra {

void ApplyLogarithmRules(std::unique_ptr<Expr>& expr)
{
	// log(ab) --> log(a) + log(b)
	LogarithmProduct(expr);
	// log(a^n) --> nlog(a)
	LogarithmPower(expr);
	// log(1) --> 0, loga(a) --> 1, loga(a^b) --> b, a^(loga(b)) --> b
	SimplifySpecialLogarithm(expr);
}

// log(ab) --> log(a) + log(b)
void LogarithmProduct(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			LogarithmProduct(expr->ChildAt(i));
	}
	else
	{
		if (!expr->LeftIsTerminal())
			LogarithmProduct(expr->Left());
		if (!expr->RightIsTerminal())
			LogarithmProduct(expr->Right());
	}

	if (!expr->IsLog())
		return;

	if (!expr->Param()->IsMul())
		return;

	if (expr->Param()->IsGeneric())
		LogarithmProductHelper(expr, true);
	else
		LogarithmProductHelper(expr, false);
}

void LogarithmProductHelper(std::unique_ptr<Expr>& expr, bool generic)
{
	if (generic)
	{
		std::unique_ptr<Expr> add_node = std::make_unique<Add>();
		int children_size = expr->Param()->ChildrenSize();

		for (int i = 0; i < children_size; i++)
		{
			std::unique_ptr<Expr> new_child;

			if (expr->IsLn())
				new_child = std::make_unique<Ln>(std::move(expr->Param()->ChildAt(i)));
			else
			{
				std::unique_ptr<Expr> base_copy;
				tree_util::DeepCopy(base_copy, expr->Base());

				new_child = std::make_unique<Log>(std::move(expr->Param()->ChildAt(i)), std::move(base_copy));
			}

			add_node->AddChild(std::move(new_child));
		}

		expr = std::move(add_node);
	}
	else
	{
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;

		if (expr->IsLn())
		{
			left = std::make_unique<Ln>(std::move(expr->Param()->Left()));
			right = std::make_unique<Ln>(std::move(expr->Param()->Right()));
		}
		else
		{
			if (expr->Base()->Name() == "10")
			{
				left = std::make_unique<Log>(std::move(expr->Param()->Left()), std::make_unique<Integer>(10));
				right = std::make_unique<Log>(std::move(expr->Param()->Right()), std::make_unique<Integer>(10));
			}
			else
			{
				left = std::make_unique<Log>(std::move(expr->Param()->Left()), std::make_unique<Integer>(2));
				right = std::make_unique<Log>(std::move(expr->Param()->Right()), std::make_unique<Integer>(2));
			}
		}

		expr = std::move(std::make_unique<Add>(std::move(left), std::move(right)));
	}
}

// log(a^n) --> nlog(a)
void LogarithmPower(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			LogarithmPower(expr->ChildAt(i));
	}
	else
	{
		if (!expr->LeftIsTerminal())
			LogarithmPower(expr->Left());
		if (!expr->RightIsTerminal())
			LogarithmPower(expr->Right());
	}

	if (!expr->IsLog())
		return;

	if (!expr->Param()->IsPow())
		return;

	if (expr->Param()->Right()->IsOne())
		return;

	if (expr->Param()->Right()->IsZero())
		return;

	std::unique_ptr<Expr> coefficient = std::move(expr->Param()->Right());
	std::unique_ptr<Expr> new_log;

	if (expr->IsLn())
		new_log = std::make_unique<Ln>(std::move(expr->Param()->Left()));
	else
	{
		if (expr->Base()->Name() == "10")
			new_log = std::make_unique<Log>(std::move(expr->Param()->Left()), std::make_unique<Integer>(10));
		else
			new_log = std::make_unique<Log>(std::move(expr->Param()->Left()), std::make_unique<Integer>(2));
	}

	std::unique_ptr<Expr> new_expr = std::make_unique<Mul>(std::move(coefficient), std::move(new_log));
	expr = std::move(new_expr);
}

// log(1) --> 0, loga(a) --> 1, loga(a^b) --> b, a^(loga(b)) --> b
void SimplifySpecialLogarithm(std::unique_ptr<Expr>& expr)
{
	if (expr->IsTerminal())
		return;

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			SimplifySpecialLogarithm(expr->ChildAt(i));
	}
	else
	{
		if (!expr->LeftIsTerminal())
			SimplifySpecialLogarithm(expr->Left());
		if (!expr->RightIsTerminal())
			SimplifySpecialLogarithm(expr->Right());
	}

	// a^(loga(b)) --> b
	if (RaisedToLog(expr))
	{
		if (expr->Left() == expr->Right()->Base())
			expr = std::move(expr->Right()->Param());
	}

	if (!expr->IsLog())
		return;

	// log 1 = 0
	if (expr->Param()->IsOne())
		expr = std::move(std::make_unique<Integer>(0));
	// loga(a) = 1
	else if (expr->Param() == expr->Base())
		expr = std::move(std::make_unique<Integer>(1));
	// loga(a^b) --> b
	else if (expr->Param()->IsPow())
	{
		if (expr->Param()->Left() == expr->Base())
			expr = std::move(expr->Param()->Right());
	}
}

bool RaisedToLog(const std::unique_ptr<Expr>& expr)
{
	if (!expr->IsPow())
		return false;

	if (!expr->Right()->IsLog())
		return false;

	return true;
}

}
