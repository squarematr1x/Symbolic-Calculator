#include "ExprTree.h"
#include "Scanner.h"

namespace yaasc
{

std::unique_ptr<Expr> ExprTree::Construct(std::string input)
{
	scanner::HandleInput(input);
	std::stack<std::unique_ptr<Expr>> expr_stack;
	int length = input.length();

	for (int i = 0; i < length; i++)
	{
		while (input[i] == ' ')
			i++;

		if (scanner::IsVariable(input[i]))
			HandleVariableInput(input, i, expr_stack);
		else if (isdigit(input[i]))
			HandleDigitInput(input, i, expr_stack);

		if (expr_stack.size() == 1)
		{
			if (input[i] == '#')
				HandleFunctionInput(input, i, expr_stack);
			else if (input[i] == '!')
			{
				std::unique_ptr<Expr> stack_top = std::move(expr_stack.top());
				expr_stack.pop();
				expr_stack.push(std::make_unique<Fac>(std::move(stack_top)));
			}
		}

		if (expr_stack.size() > 1)
		{
			if (input[i] == '+')
				UpdateStack(expr_stack, ExprType::ADD);
			else if (input[i] == '*')
				UpdateStack(expr_stack, ExprType::MUL);
			else if (input[i] == '/')
				UpdateStack(expr_stack, ExprType::DIV);
			else if (input[i] == '^')
				UpdateStack(expr_stack, ExprType::POW);
			else if (input[i] == '#')
				UpdateFunctionStack(input, i, expr_stack);
			else if (input[i] == '!')
				UpdateStack(expr_stack, ExprType::FAC);
			else if (input[i] == '-')
			{
				if (i + 1 < length)
				{
					if (input[i + 1] == ' ')
						UpdateStack(expr_stack, ExprType::SUB);
				}
				else
					UpdateStack(expr_stack, ExprType::SUB);
			}
		}
		else if (expr_stack.size() == 1)
		{
			if (input[i] == '-')
			{
				if (i + 1 < length && input[i + 1] == ' ')
				{
					std::unique_ptr<Expr> expr = std::move(expr_stack.top());
					expr_stack.pop();
					TurnToNegative(expr);
					expr_stack.push(std::move(expr));
				}
			}
		}
	}

	if (!expr_stack.empty())
	{
		std::unique_ptr<Expr> final_tree = std::move(expr_stack.top());
		expr_stack.pop();

		return final_tree;
	}

	return nullptr;
}

void ExprTree::HandleVariableInput(std::string input, int& index, std::stack<std::unique_ptr<Expr>>& expr_stack)
{
	std::unique_ptr<Expr> expr;

	if (input[index] == 'e')
		expr = std::make_unique<E>();
	else if (input[index] == '~')
		expr = std::make_unique<Pi>();
	else
		expr = std::make_unique<Var>(std::string(1, input[index]));

	expr = std::make_unique<Pow>(std::move(expr), std::make_unique<Integer>(1));

	if (index - 1 >= 0 && input[index - 1] == '-')
		expr_stack.push(std::make_unique<Mul>(std::make_unique<Integer>(-1), std::move(expr)));
	else
		expr_stack.push(std::move(expr));
}

void ExprTree::HandleDigitInput(std::string input, int& index, std::stack<std::unique_ptr<Expr>>& expr_stack)
{
	int length = input.length();
	int first_index = index;
	bool is_float = false;
	std::string number = "";
	number += input[index];

	for (int i = index + 1; i < length; i++)
	{
		if (isdigit(input[i]))
			number += input[i];
		else if (input[i] == '.' && is_float == false)
		{
			number += input[i];
			is_float = true;
		}
		else
		{
			index = i - 1;
			break;
		}
	}

	if (!is_float)
	{
		int value = std::stoi(number);

		if (first_index - 1 >= 0 && input[first_index - 1] == '-')
			expr_stack.push(std::make_unique<Integer>(value * -1));
		else
			expr_stack.push(std::make_unique<Integer>(value));
	}
	else
	{
		float value = std::stof(number);

		if (first_index - 1 >= 0 && input[first_index - 1] == '-')
			expr_stack.push(std::make_unique<Float>(value * -1.0f));
		else
			expr_stack.push(std::make_unique<Float>(value));
	}
}

void ExprTree::HandleFunctionInput(std::string input, int& index, std::stack<std::unique_ptr<Expr>>& expr_stack)
{
	++index;
	std::string func_name = FuncName(input, index);
	std::unique_ptr<Expr> stack_top = std::move(expr_stack.top());
	expr_stack.pop();

	AddFunctionToStack(func_name, stack_top, expr_stack);
}

void ExprTree::UpdateFunctionStack(std::string input, int& index, std::stack<std::unique_ptr<Expr>>& expr_stack)
{
	++index;
	std::string func_name = FuncName(input, index);

	std::unique_ptr<Expr> right = std::move(expr_stack.top());
	expr_stack.pop();

	std::unique_ptr<Expr> left = std::move(expr_stack.top());
	expr_stack.pop();

	expr_stack.push(std::move(left));
	AddFunctionToStack(func_name, right, expr_stack);
}

void ExprTree::AddFunctionToStack(std::string func_name, std::unique_ptr<Expr>& expr, std::stack<std::unique_ptr<Expr>>& expr_stack)
{
	if (func_name == "log" || func_name == "log10")
		expr_stack.push(std::make_unique<Log>(std::move(expr), std::make_unique<Integer>(10)));
	else if (func_name == "log2")
		expr_stack.push(std::make_unique<Log>(std::move(expr), std::make_unique<Integer>(2)));
	else if (func_name == "ln")
		expr_stack.push(std::make_unique<Ln>(std::move(expr)));
	else if (func_name == "sin")
		expr_stack.push(std::make_unique<Sin>(std::move(expr)));
	else if (func_name == "cos")
		expr_stack.push(std::make_unique<Cos>(std::move(expr)));
	else if (func_name == "tan")
		expr_stack.push(std::make_unique<Tan>(std::move(expr)));
	else if (func_name == "D")
		expr_stack.push(std::make_unique<Derivative>(std::move(expr), "x")); // FIXME: Can be respect to any variable
	else if (func_name == "I")
		expr_stack.push(std::make_unique<Integral>(std::move(expr), "x")); // FIXME: Can be respect to any variable
}

std::string ExprTree::FuncName(std::string input, int& index)
{
	int end_index = 0;
	int length = input.length();
	std::string func_name = "";

	for (int i = index; i < length; i++)
	{
		if (input[i] != ' ')
			func_name += input[i];
		else
		{
			end_index = i;
			break;
		}
	}

	index = end_index;

	return func_name;
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
	else if (type == ExprType::POW)
		expr_stack.push(std::make_unique<Pow>(std::move(left), std::move(right)));
	else if (type == ExprType::DIV)
	{
		if (left->IsInteger() && right->IsInteger())
			expr_stack.push(std::make_unique<Fraction>(left->iValue(), right->iValue()));
		else
			expr_stack.push(std::make_unique<Mul>(std::move(left), std::make_unique<Pow>(std::move(right), std::make_unique<Integer>(-1))));
	}
	else if (type == ExprType::SUB)
	{
		TurnToNegative(right);
		expr_stack.push(std::make_unique<Add>(std::move(left), std::move(right)));
	}
	else if (type == ExprType::FAC)
	{
		expr_stack.push(std::move(left));
		expr_stack.push(std::make_unique<Fac>(std::move(right)));
	}
}

void ExprTree::TurnToNegative(std::unique_ptr<Expr>& expr)
{
	if (expr->IsNumber())
	{
		if (expr->IsInteger())
			expr = std::make_unique<Integer>(expr->iValue() * -1);
		else if (expr->IsFloat())
			expr = std::make_unique<Float>(expr->fValue() * -1.0f);
		else if (expr->IsFraction())
			expr = std::make_unique<Fraction>(expr->Numerator() * -1, expr->Denominator());
	}
	else
		expr = std::make_unique<Mul>(std::make_unique<Integer>(-1), std::move(expr));
}

void ExprTree::PrintParenthesis(const std::unique_ptr<Expr>& expr, const std::unique_ptr<Expr>& child, bool left_paranthesis)
{
	if ((expr->IsMul() && child->IsAdd())  || 
        (expr->IsPow() && child->IsMul())  ||
        (expr->IsPow() && child->IsAdd())  ||
        (expr->IsPow() && child->IsFunc()))
	{
		if (left_paranthesis)
			std::cout << "(";
		else
			std::cout << ")";
	}
}

void ExprTree::PrintFunction(const std::unique_ptr<Expr>& expr)
{
	std::cout << "(";
	PrintInorder(expr);
	std::cout << ")";
}

void ExprTree::PrintAssociative(const std::unique_ptr<Expr>& expr)
{
	if (expr->ChildrenSize() != 0)
	{
		for (int i = 0; i < expr->ChildrenSize(); i++)
		{
			if (expr->ChildAt(i)->IsTerminal())
			{
				if (expr->IsMul() && expr->ChildAt(i)->IsNegOne())
					std::cout << "-";
				else
					std::cout << expr->ChildAt(i);
			}
			else
			{
				PrintParenthesis(expr, expr->ChildAt(i), true);
				PrintInorder(expr->ChildAt(i));
				PrintParenthesis(expr, expr->ChildAt(i), false);
			}

			if (expr->IsAdd() && i < expr->ChildrenSize() - 1)
			{
				std::cout << "+";
			}
		}
	}
}

void ExprTree::PrintInorder(const std::unique_ptr<Expr>& expr)
{
	if (!expr) 
		return;

	if (expr->HasLeftChild())
	{
		if (expr->IsMul() && expr->Left()->IsNegOne())
			std::cout << "-";
		else
		{
			PrintParenthesis(expr, expr->Left(), true);
			PrintInorder(expr->Left());
			PrintParenthesis(expr, expr->Left(), false);
		}
	}

	if (expr->IsGeneric())
		PrintAssociative(expr);
	else
	{
		if (expr->IsFunc())
		{
			if (expr->IsFac())
			{
				PrintFunction(expr->Param());
				std::cout << expr;
			}
			else
			{
				std::cout << expr;

				if (expr->IsLog() && expr->Base()->Name() == "2")
					std::cout << "2";

				PrintFunction(expr->Param());
			}
		}
		else if (!expr->IsMul())
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

void ExprTree::PrintBinaryNodeOnly(const std::unique_ptr<Expr>& expr)
{
	if (expr == nullptr) return;

	if (expr->HasLeftChild())
		PrintBinaryNodeOnly(expr->Left());

	std::cout << expr;

	if (expr->HasRightChild())
		PrintBinaryNodeOnly(expr->Right());
}

void ExprTree::AddParenthesisToString(const std::unique_ptr<Expr>& expr, const std::unique_ptr<Expr>&  child, std::string& input, bool left_parenthesis)
{
	bool can_add_parenthesis = false;

	if (expr->IsPow() && (child->IsAdd() || child->IsMul() || child->IsFunc()))
		can_add_parenthesis = true;
	else if (expr->IsMul() && child->IsAdd())
		can_add_parenthesis = true;

	if (can_add_parenthesis)
	{
		if (left_parenthesis)
			input += '(';
		else
			input += ')';
	}
}

void ExprTree::RootToString(const std::unique_ptr<Expr>& expr, std::string& input)
{
	if (expr->HasLeftChild())
	{
		if (expr->IsMul() && expr->Left()->IsNegOne())
			input += '-';
		else
		{
			AddParenthesisToString(expr, expr->Left(), input, true);
			RootToString(expr->Left(), input);
			AddParenthesisToString(expr, expr->Left(), input, false);
		}
	}

	if (expr->IsGeneric())
	{
		int size = expr->ChildrenSize();

		for (int i = 0; i < size; i++)
		{
			AddParenthesisToString(expr, expr->ChildAt(i), input, true);

			if (expr->IsMul() && expr->ChildAt(i)->IsNegOne())
				input += '-';
			else
			{
				RootToString(expr->ChildAt(i), input);

				if (i + 1 < size && expr->IsAdd())
						input += '+';
			}

			AddParenthesisToString(expr, expr->ChildAt(i), input, false);
		}
	}
	else if (expr->IsFunc())
	{
		if (!expr->IsFac())
			input += expr->Name();

		input += '(';
		RootToString(expr->Param(), input);
		input += ')';

		if (expr->IsFac())
			input += expr->Name();
	}
	else
	{
		if (!expr->IsMul())
			input += expr->Name();
	}

	if (expr->HasRightChild())
	{
		AddParenthesisToString(expr, expr->Right(), input, true);
		RootToString(expr->Right(), input);
		AddParenthesisToString(expr, expr->Right(), input, false);
	}
}

std::string ExprTree::TreeString()
{
	std::string output = "";

	if (!m_root)
		return output;

	RootToString(std::move(m_root), output);

	std::regex pattern("\\+\\-");
	output = std::regex_replace(output, pattern, "-");
	return output;
}

} // namespace yaasc
