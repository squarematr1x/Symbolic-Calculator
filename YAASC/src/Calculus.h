#pragma once

#include "Expr.h"

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
void ChainRule(std::unique_ptr<Expr>& expr);
void ApplyChainRule(std::unique_ptr<Expr>& expr, std::unique_ptr<Expr>& mul_node, bool is_outermost);
void ApplyDerivativeRules(std::unique_ptr<Expr>& expr, bool skip_chain_rule = false);

bool IsConstant(const std::unique_ptr<Expr>& expr, std::string respect_to);
bool CanApplyChainRule(const std::unique_ptr<Expr>& expr);

}
