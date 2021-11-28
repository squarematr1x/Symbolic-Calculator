#pragma once 

#include "Expr.h"

namespace algebra {

void Expand(std::unique_ptr<Expr>& root);
void MultiplyBinNode(std::unique_ptr<Expr>& root);
void MultiplyGenNode(std::unique_ptr<Expr>& root);

bool CanMultiplyBinSumNode(const std::unique_ptr<Expr>& expr);
bool CanMultiplyGenSumNode(const std::unique_ptr<Expr>& expr);

} // namespace algebra
