#include "Scanner.h"

namespace scanner {

void HandleInput(std::string& input)
{
	CleanInput(input);
	AddMultiplySign(input);
	AddFunctionToken(input);
	AddPiToken(input);
	AddUnaryToken(input);
	InfixToPostfix(input);
	UnaryTokenOff(input);
}

void InfixToPostfix(std::string& input)
{
	std::string postfix_string = "";
	std::stack<char> operator_stack;
	std::stack<std::string> func_stack;

	for (unsigned int i = 0; i < input.length(); i++)
	{
		while (input[i] == ' ')
			++i;

		if (IsVariable(input[i]))
		{
			postfix_string += input[i];
			postfix_string += ' ';
		}
		else if (isdigit(input[i]))
		{
			if (i + 1 < input.length() && isdigit(input[i + 1]))
				postfix_string += input[i];
			else if (i + 1 < input.length() && input[i + 1] == '.')
			{
				postfix_string += input[i];
				postfix_string += input[i + 1];
				++i;
			}
			else
			{
				postfix_string += input[i];
				postfix_string += ' ';
			}
		}
		else if (input[i] == '(')
			operator_stack.push('(');
		else if (input[i] == ')')
		{
			while (!operator_stack.empty() && operator_stack.top() != '(')
				AddStackTopToString(postfix_string, operator_stack, func_stack);

			if (operator_stack.size() > 0 && operator_stack.top() == '(')
				operator_stack.pop();
		}

		else if (input[i] == '_')
			postfix_string += input[i];
		else // Operator is scanned
		{
			while (!operator_stack.empty() && Precedence(input[i]) <= Precedence(operator_stack.top()))
				AddStackTopToString(postfix_string, operator_stack, func_stack);

			operator_stack.push(input[i]);

			if (IsFunctionToken(input[i]))
			{
				++i;
				FunctionToStack(input, i, func_stack);
			}
		}
	}
	// Popping remaining elements from the stack
	while (!operator_stack.empty())
		AddStackTopToString(postfix_string, operator_stack, func_stack);

	input = postfix_string;
}

void AddUnaryToken(std::string& input)
{
	std::string new_string = "";
	int length = input.length();

	for (int i = 0; i < length; i++)
	{
		if (i - 1 > 0 && input[i] == '-')
		{
			if (!IsOperand(input[i - 1]) &&
				!IsRightParenthesis(input[i - 1]) &&
				!IsFactorial(input[i - 1]))
				new_string += '_';
			else
				new_string += input[i];
		}
		else
			new_string += input[i];
	}

	input = new_string;
}

void AddFunctionToken(std::string& input)
{
	std::string new_string = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		if (IsFunction(input, i))
		{
			if (input[i] == '*')
				new_string += input[i];

			new_string += '#';
			FunctionTokenHelper(input, i, new_string);
		}
		else
			new_string += input[i];
	}

	input = new_string;
}

void AddPiToken(std::string& input)
{
	std::string new_string = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		if (input[i] == 'p')
		{
			if (i + 2 < input.length() && input[i + 2] == 'i')
			{
				new_string += '~';
				i += 2;
			}
			else
				new_string += input[i];
		}
		else
			new_string += input[i];
	}

	input = new_string;
}

void UnaryTokenOff(std::string& input)
{
	std::string new_string = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		if (input[i] == '_')
			new_string += '-';
		else
			new_string += input[i];
	}

	input = new_string;
}

void AddMultiplySign(std::string& input)
{
	std::string new_input = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		new_input += input[i];

		if (input[i] == '#')
		{
			while (input[i] != ')')
			{
				++i;
				new_input += input[i];
			}
		}

		if (CanAddMultiplySign(input[i], input[i + 1]) && i + 1 < input.length())
			new_input += '*';
	}

	input = new_input;
}

void RemoveMultiplySign(std::string& input)
{
	std::string new_input = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		if (input[i] != '*')
			new_input += input[i];
	}

	input = new_input;
}

void CleanInput(std::string& input)
{
	std::string new_input = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		if (IsOperand(input[i]) || IsOperator(input[i]) || IsParenthesis(input[i]) || input[i] == '.')
			new_input += input[i];
	}

	input = new_input;

	CleanDuplicateOperators(input);
}

void CleanDuplicateOperators(std::string& input)
{
	std::string new_input = "";

	for (unsigned int i = 0; i < input.length(); i++)
	{
		new_input += input[i];

		if (!IsOperand(input[i]) && !IsParenthesis(input[i]))
		{
			while (i + 1 < input.length() && input[i] == input[i + 1])
				++i;
		}
	}

	input = new_input;
}

void AddStackTopToString(std::string& input, std::stack<char>& char_stack, std::stack<std::string>& func_stack)
{
	char stack_top = char_stack.top();

	if (IsFunctionToken(stack_top))
		AddFunctionToString(input, char_stack, func_stack);
	else
	{
		char_stack.pop();
		input += stack_top;
		input += ' ';
	}
}

void AddFunctionToString(std::string& input, std::stack<char>& char_stack, std::stack<std::string>& func_stack)
{
	if (!func_stack.empty())
	{
		char stack_top = char_stack.top();
		char_stack.pop();
		input += stack_top;
		std::string func_top = func_stack.top();
		func_stack.pop();
		input.append(func_top);
		input += ' ';
	}
}

void FunctionTokenHelper(std::string input, unsigned int& start_index, std::string& postfix_string)
{
	std::string function_string = "";
	int end_index = 0;

	for (unsigned int i = start_index; i < input.length(); i++)
	{
		if (input[i] != '*')
			function_string += input[i];

		if (input[i] == '(')
		{
			end_index = i;
			break;
		}
	}

	postfix_string += function_string;
	start_index = end_index;
}

void FunctionToStack(std::string input, unsigned int& start_index, std::stack<std::string>& func_stack)
{
	std::string function_string = "";
	int end_index = 0;

	for (unsigned int i = start_index; i < input.length(); i++)
	{
		if (input[i] != '(')
			function_string += input[i];
		else
		{
			end_index = --i;
			break;
		}
	}

	start_index = end_index;
	func_stack.push(function_string);
}

bool IsVariable(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return true;

	if (c == '~') // when input is pi
		return true;

	return false;
}

bool IsFactorial(char c)
{
	if (c == '!')
		return true;

	return false;
}

bool IsOperand(char c)
{
	if (IsVariable(c) || isdigit(c))
		return true;

	return false;
}

bool IsOperator(char c)
{
	if (c == '+' || c == '-' ||
		c == '*' || c == '/' ||
		c == '^' || c == '!')
		return true;

	return false;
}

bool IsFunctionToken(char c)
{
	if (c == '#')
		return true;

	return false;
}

bool IsParenthesis(char c)
{
	return IsLeftParenthesis(c) || IsRightParenthesis(c);
}

bool IsLeftParenthesis(char c)
{
	if (c == '(')
		return true;

	return false;
}

bool IsRightParenthesis(char c)
{
	if (c == ')')
		return true;

	return false;
}

bool CanAddMultiplySign(char c, char next_c)
{
	if (IsVariable(c) && IsOperand(next_c))
		return true;
	else if (IsVariable(c) && IsLeftParenthesis(next_c))
		return true;
	else if (isdigit(c) && IsVariable(next_c))
		return true;
	else if (isdigit(c) && IsLeftParenthesis(next_c))
		return true;
	else if (IsFactorial(c) && IsLeftParenthesis(next_c))
		return true;
	else if (IsFactorial(c) && IsOperand(next_c))
		return true;
	else if (IsRightParenthesis(c) && IsOperand(next_c))
		return true;
	else if (IsRightParenthesis(c) && IsLeftParenthesis(next_c))
		return true;

	return false;
}

bool MissingParenthesis(std::string input)
{
	int left_count = 0;
	int right_count = 0;

	for (unsigned int i = 0; i < input.length(); i++)
	{
		if (input[i] == '(')
			left_count++;
		else if (input[i] == ')')
			right_count++;
	}

	if (left_count == right_count)
		return false;

	return true;
}

bool IsFunction(std::string input, unsigned int start_index)
{
	std::string function_string = "";
	bool has_input = false;

	for (unsigned int i = start_index; i < input.length(); i++)
	{
		if (input[i] != '(' && input[i] != '*')
			function_string += input[i];
		else if (input[i] == '(')
		{
			has_input = HasInput(input, i);
			break;
		}
	}

	if (!has_input)
		return false;

	std::vector<std::string> functions{ "log", "log2", "log10", "ln", "sin", "cos", "tan", "D", "I" };
	bool is_function = false;

	for (auto function : functions)
	{
		if (function_string == function)
		{
			is_function = true;
			break;
		}
	}
	
	return is_function;
}

bool HasInput(std::string input, unsigned int start_index)
{
	bool has_input = false;
	bool has_left_parenthesis = false;
	bool has_right_parenthesis = false;

	for (unsigned int i = start_index; i < input.length(); i++)
	{
		if (IsOperand(input[i]) && !has_input)
		{
			if (IsFunction(input, i))
				MoveIndexToParameter(input, i);
			else
				has_input = true;
		}
		else if (IsLeftParenthesis(input[i]))
			has_left_parenthesis = true;
		else if (IsRightParenthesis(input[i]))
		{
			has_right_parenthesis = true;
			break;
		}
	}

	if (has_left_parenthesis && has_right_parenthesis && has_input)
		return true;

	return false;
}

void MoveIndexToParameter(std::string input, unsigned int& index)
{
	unsigned int new_index = 0;

	for (unsigned int i = index; i < input.length(); i++)
	{
		if (input[i] == '(')
		{
			if (i + 1 < input.length())
			{
				new_index = i;
				break;
			}
		}
	}

	index = new_index;
}

int Precedence(char c)
{
	if (c == '+' || c == '-')
		return 1;
	else if (c == '*' || c == '/')
		return 2;
	else if (c == '^')
		return 3;
	else if (c == '#' || c == '!')
		return 4;
	else if (c == '_')
		return 5;

	return -1;
}

} // namespace scanner
