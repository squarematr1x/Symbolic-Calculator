#include "Logarithm.h"

namespace algebra {

void ApplyLogarithmRules(std::unique_ptr<Expr>& expr)
{
	// log(ab) --> log(a) + log(b)
	LogarithmProduct(expr);
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
	{
		std::unique_ptr<Expr> add_node = std::make_unique<Add>();
		int children_size = expr->Param()->ChildrenSize();

		for (int i = 0; i < children_size; i++)
		{
			std::unique_ptr<Expr> new_child = std::make_unique<Log>(std::move(expr->Param()->ChildAt(i)), 
					                          std::make_unique<Integer>(10));
			add_node->AddChild(std::move(new_child));
		}
	}
	else
	{
		std::unique_ptr<Expr> left = std::make_unique<Log>(std::move(expr->Param()->Left()), std::make_unique<Integer>(10));
		std::unique_ptr<Expr> right = std::make_unique<Log>(std::move(expr->Param()->Right()), std::make_unique<Integer>(10));
		expr = std::move(std::make_unique<Add>(std::move(left), std::move(right)));
	}
}

// log(a/b) --> log(a) - log(b)
void LogarithmQuotient(std::unique_ptr<Expr>& expr)
{
	(void)expr;
}

// log(a^n) --> nlog(a)
void LogarithmPower(std::unique_ptr<Expr>& expr)
{
	(void)expr;
}

}
