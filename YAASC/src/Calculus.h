#pragma once

#include "Expr.h"
#include "TreeUtil.h"

namespace calculus {

void Differentiate(std::unique_ptr<Expr>& expr, std::string respect_to);
void Integrate(std::unique_ptr<Expr>& expr, std::string respect_to);


void PowerRule(std::unique_ptr<Expr>& expr, std::string respect_to);

}
