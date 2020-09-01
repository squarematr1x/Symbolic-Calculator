#pragma once

#include <stack>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

namespace scanner {

void HandleInput(std::string& input);
void InfixToPostfix(std::string& input);
void AddUnaryToken(std::string& input);
void UnaryTokenOff(std::string& input);
void AddMultiplySign(std::string& input);
void RemoveMultiplySign(std::string& input);
void CleanDuplicateOperators(std::string& input);
void AddStackTopToString(std::string& input, std::stack<char>& char_stack);
void AddFunction(std::string input, unsigned int& start_index, std::string& postfix_string);

bool Operand(char c);
bool Variable(char c);
bool LeftParenthesis(char c);
bool RightParenthesis(char c);
bool CanAddMultiplySign(char c, char next_c);
bool MissingParenthesis(std::string input);
bool IsFunction(std::string input, unsigned int start_index);

int Precedence(char c);

} // namespace scanner
