#pragma once

#include "Expr.h"
#include "TreeUtil.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr);
void Integrate(std::unique_ptr<Expr>& expr);

void PowerRule(std::unique_ptr<Expr>& expr);
void ExponentialRule(std::unique_ptr<Expr>& expr);
void SetToZero(std::unique_ptr<Expr>& expr);
void SetToOne(std::unique_ptr<Expr>& expr);
void CanDifferentiate(const std::unique_ptr<Expr>& expr, std::string respect_to, bool& is_constant);

void DifferentiateSum(std::unique_ptr<Expr>& expr);
void ProductRule(std::unique_ptr<Expr>& expr);
void QuotientRule(std::unique_ptr<Expr>& expr);

void ApplyDerivativeRules(std::unique_ptr<Expr>& expr);

bool IsConstant(const std::unique_ptr<Expr>& expr, std::string respect_to);

}
