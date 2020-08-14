#pragma once

#include <queue>

#include "Expr.h"
#include "TreeUtil.h"

namespace algebra {

void ExponentRuleMul(std::unique_ptr<Expr>& root);
void ApplyExponentRuleMulBinNode(std::unique_ptr<Expr>& root);
void ApplyExponentRuleMulGenNode(std::unique_ptr<Expr>& root);
void ExponentRulePow(std::unique_ptr<Expr>& root);
void ExponentRuleParenthesis(std::unique_ptr<Expr>& root);
void HandleExponentRuleParenthesis(std::unique_ptr<Expr>& base, std::unique_ptr<Expr>& exponent, bool generic);

bool CanApplyExponentRule(const std::unique_ptr<Expr>& expr, std::string value);
bool SameVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);
bool IsTerminal(const std::unique_ptr<Expr>& expr);
}
