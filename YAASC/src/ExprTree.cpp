#include "ExprTree.h"

namespace yaasc
{

std::unique_ptr<Expr> ExprTree::Construct(std::string input)
{
	scanner::HandleInput(input);
	std::stack<std::unique_ptr<Expr>> expression_stack;

	for (int i = 0; i < (int)input.length(); i++)
	{
		while (input[i] == ' ')
			i++;

		if (scanner::Variable(input[i]))
		{
			std::unique_ptr<Pow> expr = std::make_unique<Pow>(std::make_unique<Var>(std::string(1, input[i])),
				std::make_unique<Integer>(1));

			if (i - 1 >= 0 && input[i - 1] == '-')
				expression_stack.push(std::make_unique<Mul>(std::make_unique<Integer>(-1), std::move(expr)));
			else
				expression_stack.push(std::move(expr));
		}
		else if (isdigit(input[i]))
		{
			int first_index = i;
			bool is_float = false;
			std::string number = "";
			number += input[i];

			for (int j = i + 1; j < (int)input.length(); j++)
			{
				if (isdigit(input[j]))
					number += input[j];
				else if (input[j] == '.' && is_float == false)
				{
					number += input[j];
					is_float = true;
				}
				else
				{
					i = j - 1;
					break;
				}
			}

			if (!is_float)
			{
				int value = std::stoi(number);
				if (first_index - 1 >= 0 && input[first_index - 1] == '-')
					expression_stack.push(std::make_unique<Integer>(value * -1));
				else
					expression_stack.push(std::make_unique<Integer>(value));
			}
			else
			{
				float value = std::stof(number);
				if (first_index - 1 >= 0 && input[first_index - 1] == '-')
					expression_stack.push(std::make_unique<Float>(value * -1.0f));
				else
					expression_stack.push(std::make_unique<Float>(value));
			}

		}

		if (expression_stack.size() > 1)
		{
			if (input[i] == '+')
				UpdateStack(expression_stack, ExprType::ADD);
			else if (input[i] == '*')
				UpdateStack(expression_stack, ExprType::MUL);
			else if (input[i] == '/')
				UpdateStack(expression_stack, ExprType::DIV);
			else if (input[i] == '^')
				UpdateStack(expression_stack, ExprType::POW);
			else if (input[i] == '-')
			{
				if (i + 1 < (int)input.length())
				{
					if (input[i + 1] == ' ')
						UpdateStack(expression_stack, ExprType::SUB);
				}
				else
					UpdateStack(expression_stack, ExprType::SUB);
			}
		}
		else if (expression_stack.size() == 1)
		{
			if (input[i] == '-')
			{
				if (i + 1 < (int)input.length() && input[i + 1] == ' ')
				{
					std::unique_ptr<Expr> expr = std::move(expression_stack.top());
					expression_stack.pop();
					expression_stack.push(std::move(std::make_unique<Mul>(std::make_unique<Integer>(-1), std::move(expr))));
				}
			}
		}
	}

	std::unique_ptr<Expr> final_tree = std::move(expression_stack.top());
	expression_stack.pop();

	return std::move(final_tree);
}

void ExprTree::UpdateStack(std::stack<std::unique_ptr<Expr>>& expr_stack, ExprType type)
{
	std::unique_ptr<Expr> right = std::move(expr_stack.top());
	expr_stack.pop();

	std::unique_ptr<Expr> left = std::move(expr_stack.top());
	expr_stack.pop();

	if (type == ExprType::MUL)
		expr_stack.push(std::make_unique<Mul>(std::move(left), std::move(right)));
	else if (type == ExprType::ADD)
		expr_stack.push(std::make_unique<Add>(std::move(left), std::move(right)));
	else if (type == ExprType::DIV) // NOTE: Fractions can be made here
		expr_stack.push(std::make_unique<Mul>(std::move(left), std::make_unique<Pow>(std::move(right), std::make_unique<Integer>(-1))));
	else if (type == ExprType::POW)
		expr_stack.push(std::make_unique<Pow>(std::move(left), std::move(right)));
	else if (type == ExprType::SUB)
	{
		std::unique_ptr<Mul> new_expression{ std::make_unique<Mul>(std::make_unique<Integer>(-1), std::move(right)) };
		expr_stack.push(std::make_unique<Add>(std::move(left), std::move(new_expression)));
	}
}

void ExprTree::PrintParenthesis(const std::unique_ptr<Expr>& expr, const std::unique_ptr<Expr>& child, bool left_paranthesis)
{
	if ((expr->Name() == "*" && (child->Name() == "+" || child->Name() == "^"))
		|| (expr->Name() == "^" && child->Name() == "*"))
	{
		if (left_paranthesis)
			std::cout << "(";
		else
			std::cout << ")";
	}
}

void ExprTree::PrintAssociative(const std::unique_ptr<Expr>& expr, ExprType type)
{
	if (expr->ChildrenSize() != 0)
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
		{
			if (expr->ChildAt(i)->IsTerminal())
				std::cout << expr->ChildAt(i);
			else
				PrintInorder(expr->ChildAt(i));

			if (type == ExprType::ADD && i < expr->ChildrenSize() - 1)
				std::cout << "+";
		}
	}
}

void ExprTree::PrintInorder(const std::unique_ptr<Expr>& expr)
{
	if (expr == nullptr) return;

	if (expr->HasLeftChild())
	{
		PrintParenthesis(expr, expr->Left(), true);
		PrintInorder(expr->Left());
		PrintParenthesis(expr, expr->Left(), false);
	}

	if (expr->IsGeneric())
		PrintAssociative(expr, expr->ExpressionType());
	else
	{
		if (!expr->IsMul())
			std::cout << expr;
	}

	if (expr->HasRightChild())
	{
		PrintParenthesis(expr, expr->Right(), true);
		PrintInorder(expr->Right());
		PrintParenthesis(expr, expr->Right(), false);
	}
}

void ExprTree::Print()
{
	PrintInorder(m_root);
	std::cout << '\n';
}

void ExprTree::RootToString(const std::unique_ptr<Expr>& expr, std::string& input)
{
	if (expr->HasLeftChild())
		RootToString(expr->Left(), input);

	if (expr->IsGeneric())
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
			RootToString(expr->ChildAt(i), input);
	}
	else
	{
		if (!expr->IsMul())
			input += expr->Name();
	}

	if (expr->HasRightChild())
		RootToString(expr->Right(), input);
}

std::string ExprTree::TreeString()
{
	std::string output = "";
	RootToString(std::move(m_root), output);

	return output;
}

} // namespace jasc
