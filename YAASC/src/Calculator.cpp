#include "Calculator.h"

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
	if (root->HasNumberChildren())
	{
		if (root->HasFloatChild())
		{
			float left = root->Left()->fValue();
			float right = root->Right()->fValue();

			if (root->IsMul())
				root = std::make_unique<Float>(left * right);
			else if (root->IsAdd())
				root = std::make_unique<Float>(left + right);
			else if (root->IsPow())
				root = std::make_unique<Float>(pow(left, right));
		}
		else
		{
			int left = root->Left()->iValue();
			int right = root->Right()->iValue();

			if (root->IsMul())
				root = std::make_unique<Integer>(left * right);
			else if (root->IsAdd())
				root = std::make_unique<Integer>(left + right);
			else if (root->IsPow())
				root = std::make_unique<Float>(static_cast<float>(pow(left, right)));
		}
	}
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
	int total_numbers = 0;
	float value = 0.0f;
	std::unique_ptr<Expr> gen_node;

	if (isMul)
	{
		gen_node = std::make_unique<Mul>();
		value = 1.0f;
	}
	else
		gen_node = std::make_unique<Add>();

	for (int i = 0; i < expr->ChildrenSize(); i++)
	{
		if (expr->ChildAt(i)->IsNumber())
		{
			if (isMul)
				value *= expr->ChildAt(i)->fValue();
			else
				value += expr->ChildAt(i)->fValue();

			total_numbers++;
		}
		else
		{
			std::unique_ptr<Expr> child_copy;
			tree_util::DeepCopy(child_copy, expr->ChildAt(i));
			gen_node->AddChild(std::move(child_copy));
		}
	}

	if (total_numbers > 1)
	{
		expr = std::move(gen_node);
		UpdateGenNode(expr, value);
	}
}

void UpdateGenNode(std::unique_ptr<Expr>& expr, float value)
{
	if (abs(value - floor(value)) < 0.000001f)
		expr->AddChild(std::make_unique<Integer>(static_cast<int>(value)));
	else
		expr->AddChild(std::make_unique<Float>(value));

	expr->SortChildren();
}

std::unique_ptr<Expr>& AddNumbers(std::unique_ptr<Expr>& expr_a, std::unique_ptr<Expr>& expr_b)
{
	std::unique_ptr<Expr> result{ std::make_unique<Integer>(0) };

	if (expr_a->IsInteger())
	{
		if (expr_b->IsInteger())
			result = std::make_unique<Integer>(expr_a->iValue() + expr_b->iValue());
		else if (expr_b->IsFloat())
			result = std::make_unique<Float>(expr_a->iValue() + expr_b->fValue());
		else if (expr_b->IsFraction())
			result = std::make_unique<Fraction>(expr_b->Numerator() + expr_a->iValue() * expr_b->Denominator(), expr_b->Denominator());
	}

	expr_a = std::move(result);
	return expr_a;
}

std::unique_ptr<Expr>& MulNumbers(std::unique_ptr<Expr>& expr_a, std::unique_ptr<Expr>& expr_b)
{
	(void)expr_a;
	(void)expr_b;
	std::unique_ptr<Expr> result{ std::make_unique<Integer>(0) };

	return expr_a;
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

	if (!expr->Param()->IsNumber())
		return;

	double number = std::stod(expr->Param()->Name());

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
