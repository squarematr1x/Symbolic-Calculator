#pragma once

#include "TreeUtil.h"
#include "PowerTransformation.h"
#include "Calculator.h"

namespace yaasc {

void Simplify(std::unique_ptr<Expr>& expr);

void MultiplySum(std::unique_ptr<Expr>& root);
void MultiplyBinNode(std::unique_ptr<Expr>& root);
void MultiplyGenNode(std::unique_ptr<Expr>& root);

void AddVariables(std::unique_ptr<Expr>& root);
void AddBinNode(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& left, std::unique_ptr<Expr>& right);
void AddGenNode(std::unique_ptr<Expr>& root);

void PowerOfSum(std::unique_ptr<Expr>& root);

void SimplifyExponents(std::unique_ptr<Expr>& root, bool final_modification);
void ApplyExponentRules(std::unique_ptr<Expr>& root);

void RemoveMulOne(std::unique_ptr<Expr>& root);
void RemoveAdditiveZeros(std::unique_ptr<Expr>& root);
void ReduceToZero(std::unique_ptr<Expr>& root);
void ReduceToOne(std::unique_ptr<Expr>& root);

void Flatten(std::unique_ptr<Expr>& root);
void ToGeneric(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& parent, std::queue<std::unique_ptr<Expr>>& children);

void Canonize(std::unique_ptr<Expr>& root);
void CanonizeBinNode(std::unique_ptr<Expr>& root);
void CanonizeGenNode(std::unique_ptr<Expr>& root);

void ReduceGenNodeToBinNode(std::unique_ptr<Expr>& root);

bool Simplified(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);

bool CanAddGen(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b); 

bool CanMultiplyBinSumNode(const std::unique_ptr<Expr>& expr);
bool CanMultiplyGenSumNode(const std::unique_ptr<Expr>& expr);

bool SameGenericVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);
bool SameExpressionTypes(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);

bool RaisedToZero(const std::unique_ptr<Expr>& expr);
bool RaisedToOne(const std::unique_ptr<Expr>& expr);
bool IsTerminal(const std::unique_ptr<Expr>& expr);
bool IsNull(const std::unique_ptr<Expr>& expr);
bool IsMulByNumber(const std::unique_ptr<Expr>& expr);

} // namespace yaasc
