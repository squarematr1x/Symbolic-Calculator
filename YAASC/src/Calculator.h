#pragma once

#include "Expr.h"

namespace calc {

	void Calculate(std::unique_ptr<Expr>& expr);
	void CalculateBinNode(std::unique_ptr<Expr>& expr);
	void CalculateGenNode(std::unique_ptr<Expr>& expr);
	void UpdateChildren(std::unique_ptr<Expr>& root, bool isMul);
	void UpdateGenNode(std::unique_ptr<Expr>& expr, float value, int first_index, int last_index);

	void ComputeFactorial(std::unique_ptr<Expr>& expr);

} // namespace calc
