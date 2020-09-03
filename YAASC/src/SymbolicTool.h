#pragma once

#include "PowerTransformation.h"
#include "PowerOfSum.h"
#include "Expand.h"
#include "Logarithm.h"
#include "Calculus.h"

namespace yaasc {

void Simplify(std::unique_ptr<Expr>& expr);

void AddVariables(std::unique_ptr<Expr>& root);
void AddBinNode(std::unique_ptr<Expr>& root, std::unique_ptr<Expr>& left, std::unique_ptr<Expr>& right);
void AddGenNode(std::unique_ptr<Expr>& root);

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

bool SameGenericVariables(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);
bool SameExpressionTypes(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);

bool RaisedToZero(const std::unique_ptr<Expr>& expr);
bool RaisedToOne(const std::unique_ptr<Expr>& expr);
bool IsMulByNumber(const std::unique_ptr<Expr>& expr);
bool IsGenMulByNumber(const std::unique_ptr<Expr>& expr);

} // namespace yaasc
