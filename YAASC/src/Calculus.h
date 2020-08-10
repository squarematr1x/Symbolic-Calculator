#pragma once

#include "Expr.h"

class Calculus
{
public:
	void differentiate(std::unique_ptr<Expr> expr, std::string respect_to);
	void integrate(std::unique_ptr<Expr> expr, std::string respect_to);

	// Apply all the rules
};
