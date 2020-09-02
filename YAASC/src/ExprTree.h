#pragma once

#include "Expr.h"
#include "Scanner.h"

namespace yaasc
{

class ExprTree
{
private:
	std::unique_ptr<Expr> m_root;

public:
	ExprTree(std::string input)
		: m_root(std::move(Construct(input)))
	{
	}

	std::unique_ptr<Expr> Construct(std::string input);
	std::unique_ptr<Expr>& Root() { return m_root; }

	std::string FuncName(std::string input, int& index);
	std::string TreeString();

	void HandleFunctionInput(std::string input, int& index, std::stack<std::unique_ptr<Expr>>& expr_stack);
	void UpdateFunctionStack(std::string input, int& index, std::stack<std::unique_ptr<Expr>>& expr_stack);
	void AddFunctionToStack(std::string func_name, std::unique_ptr<Expr>& expr, std::stack<std::unique_ptr<Expr>>& expr_stack);
	void RootToString(const std::unique_ptr<Expr>& expr, std::string& input);
	void UpdateStack(std::stack<std::unique_ptr<Expr>>& expr_stack, ExprType type);
	void ReplaceRoot(std::unique_ptr<Expr> new_root) { m_root = std::move(new_root); }

	void PrintAssociative(const std::unique_ptr<Expr>& expr);
	void PrintParenthesis(const std::unique_ptr<Expr>& expr, const std::unique_ptr<Expr>& child, bool left_parenthesis);
	void PrintFunction(const std::unique_ptr<Expr>& expr);
	void PrintInorder(const std::unique_ptr<Expr>& expr);
	void PrintBinaryNodeOnly(const std::unique_ptr<Expr>& expr);
	void Print();
};

} // namespace yaasc
