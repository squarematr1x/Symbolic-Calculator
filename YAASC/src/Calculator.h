#pragma once

#include <cmath>
#include <string>

#include "Expr.h"
#include "TreeUtil.h"

namespace calc {

void Calculate(std::unique_ptr<Expr>& expr);
void CalculateBinNode(std::unique_ptr<Expr>& expr);
void CalculateGenNode(std::unique_ptr<Expr>& expr);
void UpdateChildren(std::unique_ptr<Expr>& root, bool isMul);

std::unique_ptr<Expr>& AddNumbers(std::unique_ptr<Expr>& expr_a, std::unique_ptr<Expr>& expr_b);
std::unique_ptr<Expr>& MulNumbers(std::unique_ptr<Expr>& expr_a, std::unique_ptr<Expr>& expr_b);

void ReduceFraction(std::unique_ptr<Expr>& expr);
void ComputeFactorial(std::unique_ptr<Expr>& expr);
void ComputeLogarithm(std::unique_ptr<Expr>& expr);
void ComputeTrigonometric(std::unique_ptr<Expr>& expr);

int Factorial(int number);

} // namespace calc
