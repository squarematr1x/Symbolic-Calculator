#include<fstream>

#include "SymbolicTool.h"
#include "ExprTree.h"
#include "Clear.h"

int main()
{
	std::cout << "Welcome to use YAASC\n";
	std::cout << "Source code at: https://github.com/squarematr1x/YAASC";
	std::cout << "\nEnter the expression you want to simplify\n\n";

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

			yaasc::Simplify(expr_tree.Root());
			output = expr_tree.TreeString();

			if (output == "")
				std::cout << "\t couldn't simplify input";
			else if (input != output)
			{
				if (std::regex_search(input, std::regex("[a-zA-Z]+")))
					std::cout << "\t simplified: ";
				else
					std::cout << "\t ans: ";
			}
			else
				std::cout << "\t couldn't simplify further: ";

			std::cout << expr_tree.TreeString() << '\n';
		}

		i++;
	}

	return 0;
}
