#include "TreeUtil.h"

namespace tree_util {

/* Transferring content of a queue into an associative (+ or *) node:
*
*                         +
*                       / | \
*  Q: [3, a, b^2] -->  3  a  ^
*                           / \
*                          b   2
*/
void MoveQueueToGenericNode(std::unique_ptr<Expr>& expr, std::queue<std::unique_ptr<Expr>>& expr_queue)
{
	while (!expr_queue.empty())
	{
		std::unique_ptr<Expr> front_expr = std::move(expr_queue.front());
		expr_queue.pop();
		expr->AddChild(std::move(front_expr));
	}
}

void DeepCopy(std::unique_ptr<Expr>& to_expr, const std::unique_ptr<Expr>& from_expr)
{
	std::stack<std::unique_ptr<Expr>> expr_stack;

	if (from_expr)
		CopyToStack(expr_stack, from_expr);

	if (!expr_stack.empty())
	{
		to_expr = std::move(expr_stack.top());

		while (!expr_stack.empty())
			expr_stack.pop();
	}
}

void CopyToStack(std::stack<std::unique_ptr<Expr>>& expr_stack, const std::unique_ptr<Expr>& expr)
{
	if (expr->HasLeftChild())
		CopyToStack(expr_stack, expr->Left());

	if (expr->HasRightChild())
		CopyToStack(expr_stack, expr->Right());

	if (expr->IsTerminal())
	{
		if (expr->IsVar())
			expr_stack.push(std::make_unique<Var>(expr->Name()));
		else if (expr->IsInteger())
			expr_stack.push(std::make_unique<Integer>(std::stoi(expr->Name())));
		else if (expr->IsFloat())
			expr_stack.push(std::make_unique<Float>(std::stof(expr->Name())));
	}
	else if (expr->IsFunc())
	{
		std::stack<std::unique_ptr<Expr>> sub_stack;
		CopyToStack(sub_stack, expr->Param());
		std::unique_ptr<Expr> parameter = std::move(sub_stack.top());
		sub_stack.pop();

		if (expr->IsFac())
			expr_stack.push(std::make_unique<Fac>(std::move(parameter)));
		else if (expr->IsLog() && expr->Base()->Name() == "10")
			expr_stack.push(std::make_unique<Log>(std::move(parameter), std::make_unique<Integer>(10)));
		else if (expr->IsLog() && expr->Base()->Name() == "2")
			expr_stack.push(std::make_unique<Log>(std::move(parameter), std::make_unique<Integer>(2)));
		else if (expr->IsLn())
			expr_stack.push(std::make_unique<Ln>(std::move(parameter)));
		else if (expr->IsSin())
			expr_stack.push(std::make_unique<Sin>(std::move(parameter)));
		else if (expr->IsCos())
			expr_stack.push(std::make_unique<Cos>(std::move(parameter)));
		else if (expr->IsTan())
			expr_stack.push(std::make_unique<Tan>(std::move(parameter)));
		else if (expr->IsDerivative())
			expr_stack.push(std::make_unique<Derivative>(std::move(parameter), "x"));
		else if (expr->IsIntegral())
			expr_stack.push(std::make_unique<Integral>(std::move(parameter), "x"));
	}
	else if (!expr->IsGeneric())
	{
		std::unique_ptr<Expr> right = nullptr;
		std::unique_ptr<Expr> left = nullptr;

		if (!expr_stack.empty())
		{
			right = std::move(expr_stack.top());
			expr_stack.pop();
		}

		if (!expr_stack.empty())
		{
			left = std::move(expr_stack.top());
			expr_stack.pop();
		}

		if (right && left)
		{
			if (expr->IsMul())
				expr_stack.push(std::make_unique<Mul>(std::move(left), std::move(right)));
			else if (expr->IsAdd())
				expr_stack.push(std::make_unique<Add>(std::move(left), std::move(right)));
			else if (expr->IsPow())
				expr_stack.push(std::make_unique<Pow>(std::move(left), std::move(right)));
		}
	}
	else
	{
		if (expr->IsMul())
			expr_stack.push(std::make_unique<Mul>());
		else
			expr_stack.push(std::make_unique<Add>());

		std::queue<std::unique_ptr<Expr>> expr_queue;
		std::stack<std::unique_ptr<Expr>> sub_stack; // Used for each children in queue

		for (int i = 0; i < expr->ChildrenSize(); i++)
			CopyToQueue(expr_queue, sub_stack, expr->ChildAt(i));

		tree_util::MoveQueueToGenericNode(expr_stack.top(), expr_queue);
	}
}

void CopyToQueue(std::queue<std::unique_ptr<Expr>>& expr_queue, std::stack<std::unique_ptr<Expr>>& sub_stack, const std::unique_ptr<Expr>& expr)
{
	CopyToStack(sub_stack, expr);

	if (!sub_stack.empty())
		expr_queue.push(std::move(sub_stack.top()));

	while (!sub_stack.empty())
		sub_stack.pop();
}

} // namespace tree_util
