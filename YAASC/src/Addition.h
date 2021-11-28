#pragma once

#include <memory>

class Expr;

namespace algebra {

void AddVariables(std::unique_ptr<Expr>& expr);
void AddBinNodes(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& left, std::unique_ptr<Expr>& right);
void AddGenNode(std::unique_ptr<Expr>& expr);
void AddGenNodes(std::unique_ptr<Expr>& root, int index, std::unique_ptr<Expr>& child_a, std::unique_ptr<Expr>& child_b);

bool IsMultipliedByNumber(const std::unique_ptr<Expr>& expr);
bool IsGenMultipliedByNumber(const std::unique_ptr<Expr>& expr);
bool CanAddGenNode(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);
bool SameGenVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);

}
