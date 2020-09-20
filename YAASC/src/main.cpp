#include<fstream>

#include "SymbolicTool.h"
#include "ExprTree.h"
#include "Clear.h"

int main()
{
	std::cout << "Welcome to use YAASC.\n";
	std::cout << "Source code at: https://github.com/squarematr1x/YAASC";
	std::cout << "\n\n";

	std::string input = "";
	std::string output = "";

	int i = 1;

	while (true)
	{
		std::cout << "yaasc:" << i << "> ";
		std::getline(std::cin, input);

		if (input == "exit")
			break;

		if (input == "clear")
			ClearScreen();
		else if (input == "help")
		{
			std::string line;
			std::ifstream help_file("..\\help.txt");

			if (help_file.is_open())
			{
				while (getline(help_file, line))
					std::cout << line << '\n';

				help_file.close();
				std::cout << '\n';
			}
			else 
				std::cout << "Unable to open help file\n";
		}
		else if (input.length() != 0 && !scanner::MissingParenthesis(input))
		{
			yaasc::ExprTree expr_tree(input);
			// expr_tree.ReplaceRoot(std::move(calc::MulNumbers(expr_tree.Root()->Left(), expr_tree.Root()->Right())));
			yaasc::Simplify(expr_tree.Root());
			output = expr_tree.TreeString();

			if (output == "")
				std::cout << "\t couldn't simplify input";
			else if (input != output)
				std::cout << "\t simplified: ";
			else
				std::cout << "\t couldn't simplify further: ";

			expr_tree.Print();
		}

		i++;
	}

	return 0;
}
