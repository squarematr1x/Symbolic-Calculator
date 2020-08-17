#pragma once

#include "Expr.h"
#include "TreeUtil.h"

namespace algebra {

void PowerOfSum(std::unique_ptr<Expr>& expr);

bool CanApplyPowerOfSum(const std::unique_ptr<Expr>& expr);

}
