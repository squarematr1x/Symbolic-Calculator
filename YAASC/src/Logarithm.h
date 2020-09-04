#pragma once

#include "Expr.h"
#include "TreeUtil.h"

namespace algebra {

void ApplyLogarithmRules(std::unique_ptr<Expr>& expr);
void LogarithmProduct(std::unique_ptr<Expr>& expr);
void LogarithmProductHelper(std::unique_ptr<Expr>& expr, bool generic);
void LogarithmQuotient(std::unique_ptr<Expr>& expr);
void LogarithmPower(std::unique_ptr<Expr>& expr);
void SimplifySpecialLogarithm(std::unique_ptr<Expr>& expr);

}
