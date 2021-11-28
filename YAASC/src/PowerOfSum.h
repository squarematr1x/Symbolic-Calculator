#pragma once

#include "Expr.h"

namespace algebra {

void PowerOfSum(std::unique_ptr<Expr>& expr);
void ApplyBinomialTheorem(std::unique_ptr<Expr>& expr);
void ApplyMultinomialTheorem(std::unique_ptr<Expr>& expr);

bool CanApplyPowerOfSum(const std::unique_ptr<Expr>& expr);

} // namespace algebra
