#include "Calculator.h"
#include "TreeUtil.h"

#include <cmath>
#include <string>

namespace calc {

void Calculate(std::unique_ptr<Expr>& root)
{
	if (root->IsTerminal())
		return;

	if (!root->LeftIsTerminal())
		Calculate(root->Left());

	if (!root->RightIsTerminal())
		Calculate(root->Right());

	if (root->IsFunc())
	{
		Calculate(root->Param());

		ComputeFactorial(root);
		ComputeLogarithm(root);
		ComputeTrigonometric(root);
	}
	else if (root->IsGeneric())
	{
		for (int i = 0; i < root->ChildrenSize(); i++)
			Calculate(root->ChildAt(i));

		CalculateGenNode(root);
	}
	else
		CalculateBinNode(root);
}

void CalculateBinNode(std::unique_ptr<Expr>& root)
{
	if (!root->HasNumberChildren())
		return;
	
	if (root->IsMul())
		root = std::move(MulNumbers(root->Left(), root->Right()));
	else if (root->IsAdd())
		root = std::move(AddNumbers(root->Left(), root->Right()));
	else if (root->IsPow())
		root = std::make_unique<Float>(pow(root->Left()->fValue(), root->Right()->fValue()));
}

void CalculateGenNode(std::unique_ptr<Expr>& root)
{
	if (root->IsMul())
		UpdateChildren(root, true);
	else
		UpdateChildren(root, false);
}

void UpdateChildren(std::unique_ptr<Expr>& expr, bool isMul)
{
	std::unique_ptr<Expr> previous_number = nullptr;
	std::unique_ptr<Expr> gen_node;

	if (isMul)
		gen_node = std::make_unique<Mul>();
	else
		gen_node = std::make_unique<Add>();

	for (int i = 0; i < expr->ChildrenSize(); i++)
	{
		if (expr->ChildAt(i)->IsNumber())
		{
			if (!previous_number)
				previous_number = std::move(expr->ChildAt(i));
			else
			{
				if (isMul)
					previous_number = std::move(MulNumbers(previous_number, expr->ChildAt(i)));
				else
					previous_number = std::move(AddNumbers(previous_number, expr->ChildAt(i)));
			}
		}
		else
		{
			std::unique_ptr<Expr> child_copy;
			tree_util::Clone(child_copy, expr->ChildAt(i));
			gen_node->AddChild(std::move(child_copy));
		}
	}

	if (previous_number)
	{
		gen_node->AddChild(std::move(previous_number));
		gen_node->SortChildren();
	}

	expr = std::move(gen_node);
}

std::unique_ptr<Expr>& AddNumbers(std::unique_ptr<Expr>& expr_a, std::unique_ptr<Expr>& expr_b)
{
	std::unique_ptr<Expr> result{ std::make_unique<Integer>(0) };

	if (expr_a->IsFloat() || expr_b->IsFloat())
		result = std::make_unique<Float>(expr_a->fValue() + expr_b->fValue());
	else if (expr_a->IsInteger())
	{
		if (expr_b->IsInteger())
			result = std::make_unique<Integer>(expr_a->iValue() + expr_b->iValue());
		else if (expr_b->IsFraction())
			result = std::make_unique<Fraction>(expr_b->Numerator() + expr_a->iValue() * expr_b->Denominator(), expr_b->Denominator());
	}
	else if (expr_a->IsFraction())
	{
		if (expr_b->IsInteger())
			result = std::make_unique<Fraction>(expr_a->Numerator() + expr_b->iValue() * expr_a->Denominator(), expr_a->Denominator());
		else if (expr_b->IsFraction())
		{
			int numerator = expr_a->Numerator() * expr_b->Denominator() + expr_b->Numerator() * expr_a->Denominator();
			int denominator = expr_a->Denominator() * expr_b->Denominator();
			result = std::make_unique<Fraction>(numerator, denominator);
		}
	}

	if (result->IsFraction())
		ReduceFraction(result);

	expr_a = std::move(result);
	return expr_a;
}

std::unique_ptr<Expr>& MulNumbers(std::unique_ptr<Expr>& expr_a, std::unique_ptr<Expr>& expr_b)
{
	std::unique_ptr<Expr> result{ std::make_unique<Integer>(1) };

	if (expr_a->IsFloat() || expr_b->IsFloat())
		result = std::make_unique<Float>(expr_a->fValue() * expr_b->fValue());
	else if (expr_a->IsInteger())
	{
		if (expr_b->IsInteger())
			result = std::make_unique<Integer>(expr_a->iValue() * expr_b->iValue());
		else if (expr_b->IsFraction())
			result = std::make_unique<Fraction>(expr_b->Numerator() * expr_a->iValue(), expr_b->Denominator());
	}
	else if (expr_a->IsFraction())
	{
		if (expr_b->IsInteger())
			result = std::make_unique<Fraction>(expr_a->Numerator() * expr_b->iValue(), expr_a->Denominator());
		else if (expr_b->IsFraction())
		{
			int numerator = expr_a->Numerator() * expr_b->Numerator();
			int denominator = expr_a->Denominator() * expr_b->Denominator();
			result = std::make_unique<Fraction>(numerator, denominator);
		}
	}

	if (result->IsFraction())
		ReduceFraction(result);

	expr_a = std::move(result);

	return expr_a;
}

void ReduceFraction(std::unique_ptr<Expr>& expr)
{
	int smaller_fraction = 0;
	int larger_fraction = 0;
	int result = -1;

	if (std::abs(expr->Numerator()) == 1 && expr->Denominator() > 1)
		return;
	else if (std::abs(expr->Numerator()) > std::abs(expr->Denominator()))
	{
		larger_fraction = std::abs(expr->Numerator());
		smaller_fraction = std::abs(expr->Denominator());
	}
	else
	{
		larger_fraction = std::abs(expr->Denominator());
		smaller_fraction = std::abs(expr->Numerator());
	}

	if (larger_fraction % smaller_fraction == 0)
		expr = std::make_unique<Integer>(expr->Numerator() / expr->Denominator());
	else
	{
		while (true)
		{
			result = larger_fraction - smaller_fraction;

			if (result == 0)
			{
				result = smaller_fraction;
				break;
			}

			if (result > smaller_fraction)
				larger_fraction = result;
			else
			{
				larger_fraction = smaller_fraction;
				smaller_fraction = result;
			}
		}

		int numerator = expr->Numerator() / result;
		int denominator = expr->Denominator() / result;

		expr = std::make_unique<Fraction>(numerator, denominator);
	}
}

void ComputeFactorial(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsFac())
		return;

	if (expr->Param()->IsInteger())
	{
		int result = 1;
		int lim = expr->Param()->iValue();

		if (lim < 0)
		{
			result = -1;
			lim = abs(lim);
		}

		for (int i = 1; i <= lim; i++)
			result *= i;

		expr = std::make_unique<Integer>(result);
	}
}

void ComputeLogarithm(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsLog())
		return;

	if (!expr->Param()->IsNumber())
		return;

	double number = std::stod(expr->Param()->Name());

	if (expr->Base()->Name() == "e")
	{
		number = log(number);
		expr = std::make_unique<Float>(std::move(static_cast<float>(number)));
	}
	else if (expr->Base()->Name() == "10")
	{
		number = log10(number);
		expr = std::make_unique<Float>(std::move(static_cast<float>(number)));
	}
	else if (expr->Base()->Name() == "2")
	{
		number = log2(number);
		expr = std::make_unique<Float>(std::move(static_cast<float>(number)));
	}
}

void ComputeTrigonometric(std::unique_ptr<Expr>& expr)
{
	if (!expr->IsTrig())
		return;

	if (expr->Param()->IsFunc())
		return;

	double number = 0;

	if (!expr->Param()->IsNumber())
	{
		if (expr->Param()->IsPow())
		{
			if (expr->Param()->Left()->IsPi())
				number = expr->Param()->Left()->fValue();
			else
				return;
		}
	}
	else
		number = expr->Param()->fValue();

	if (expr->IsSin())
	{
		number = sin(number);
		expr = std::make_unique<Float>(std::move(static_cast<float>(number)));
	}
	else if (expr->IsCos())
	{
		number = cos(number);
		expr = std::make_unique<Float>(std::move(static_cast<float>(number)));
	}
	else if (expr->IsTan())
	{
		number = tan(number);
		expr = std::make_unique<Float>(std::move(static_cast<float>(number)));
	}
}

int Factorial(int number)
{
	if (number == 0 || number == 1)
		return 1;

	int result = 1;

	for (int i = 1; i <= number; i++)
		result *= i;

	return result;
}

} //namespace calc
