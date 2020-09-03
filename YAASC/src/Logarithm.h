#pragma once

#include "Expr.h"

namespace algebra {

void ApplyLogarithmRules(std::unique_ptr<Expr>& expr);
void LogarithmProduct(std::unique_ptr<Expr>& expr);
void LogarithmQuotient(std::unique_ptr<Expr>& expr);
void LogarithmPower(std::unique_ptr<Expr>& expr);
void SimplifySpecialLogarithm(std::unique_ptr<Expr>& expr);

}
