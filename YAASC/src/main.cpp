#include "SymbolicTool.h"
#include "ExprTree.h"

int main()
{
	std::string input = "";
	std::string output = "";

	while (true)
	{
		std::cout << ">>>";
		std::getline(std::cin, input);

		if (input == "exit")
			break;

		if (input.length() != 0 && !scanner::MissingParenthesis(input))
		{
			yaasc::ExprTree expr_tree(input);
			yaasc::Simplify(expr_tree.Root());
			output = expr_tree.TreeString();

			if (input != output)
				std::cout << "\t simplified: ";
			else
				std::cout << "\t couldn't simplify further: ";

			expr_tree.Print();
		}
	}

	return 0;
}
