#pragma once

#include "Expr.h"
#include "TreeUtil.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr);
void Integrate(std::unique_ptr<Expr>& expr);

void PowerRule(std::unique_ptr<Expr>& expr);
void SetToZero(std::unique_ptr<Expr>& expr);
void SetToOne(std::unique_ptr<Expr>& expr);
void CanDifferentiate(const std::unique_ptr<Expr>& root, const std::unique_ptr<Expr>& expr, bool& is_constant);

void DifferentiateSum(std::unique_ptr<Expr>& expr);
void DifferentiateMul(std::unique_ptr<Expr>& expr);

void DerivativeRules(std::unique_ptr<Expr>& expr);

}
