#pragma once

#include <queue>
#include <stack>

#include "Expr.h"

namespace tree_util {

void MoveQueueToGenericNode(std::unique_ptr<Expr>& expr, std::queue<std::unique_ptr<Expr>>& expr_queue);
void Clone(std::unique_ptr<Expr>& to_expr, const std::unique_ptr<Expr>& from_expr);
void CopyToStack(std::stack<std::unique_ptr<Expr>>& expr_stack, const std::unique_ptr<Expr>& expr);
void CopyToQueue(std::queue<std::unique_ptr<Expr>>& expr_queue, std::stack<std::unique_ptr<Expr>>& sub_stack, const std::unique_ptr<Expr>& expr);

}
