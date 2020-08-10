#pragma once

#include <string>
#include <algorithm>
#include <stack>

namespace scanner {

void HandleInput(std::string& input);
void InfixToPostfix(std::string& input);
void AddUnaryToken(std::string& input);
void UnaryTokenOff(std::string& input);
void AddMultiplySign(std::string& input);
void RemoveMultiplySign(std::string& input);
void CleanDuplicateOperators(std::string& input);
void AddStackTopToString(std::string& input, std::stack<char>& char_stack);

bool Operand(char c);
bool Variable(char c);
bool LeftParenthesis(char c);
bool RightParenthesis(char c);
bool CanAddMultiplySign(char c, char next_c);
bool MissingParenthesis(std::string input);

int Precedence(char c);

} // namespace scanner
