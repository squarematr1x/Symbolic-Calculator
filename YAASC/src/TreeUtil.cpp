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
	else if (!expr->IsGeneric())
	{
		std::unique_ptr<Expr> right = std::move(expr_stack.top());
		expr_stack.pop();
		std::unique_ptr<Expr> left = std::move(expr_stack.top());
		expr_stack.pop();

		if (expr->IsMul())
			expr_stack.push(std::make_unique<Mul>(std::move(left), std::move(right)));
		else if (expr->IsAdd())
			expr_stack.push(std::make_unique<Add>(std::move(left), std::move(right)));
		else if (expr->IsPow())
			expr_stack.push(std::make_unique<Pow>(std::move(left), std::move(right)));
	}
	else
	{
		if (expr->IsMul())
			expr_stack.push(std::make_unique<Mul>(nullptr, nullptr));
		else
			expr_stack.push(std::make_unique<Add>(nullptr, nullptr));

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
	expr_queue.push(std::move(sub_stack.top()));

	while (!sub_stack.empty())
		sub_stack.pop();
}

} // namespace tree_util
