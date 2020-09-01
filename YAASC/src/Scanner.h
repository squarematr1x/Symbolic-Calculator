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
void AddFunctionToken(std::string& input);
void UnaryTokenOff(std::string& input);
void AddMultiplySign(std::string& input);
void RemoveMultiplySign(std::string& input);
void CleanDuplicateOperators(std::string& input);
void AddStackTopToString(std::string& input, std::stack<char>& char_stack, std::stack<std::string>& func_stack);
void AddFunctionToString(std::string& input, std::stack<char>& char_stack, std::stack<std::string>& func_stack);
void FunctionTokenHelper(std::string input, unsigned int& start_index, std::string& postfix_string);
void FunctionToStack(std::string input, unsigned int& start_index, std::stack<std::string>& func_stack);

bool IsOperand(char c);
bool IsVariable(char c);
bool IsFunctionToken(char c);
bool IsParenthesis(char c);
bool IsLeftParenthesis(char c);
bool IsRightParenthesis(char c);
bool CanAddMultiplySign(char c, char next_c);
bool MissingParenthesis(std::string input);
bool IsFunction(std::string input, unsigned int start_index);

int Precedence(char c);

} // namespace scanner
