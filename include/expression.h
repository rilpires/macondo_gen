#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "utils.h"

enum EXPRESSION_TOKENS
{
  EXPRESSION_VARIABLE,
  EXPRESSION_CONSTANT,
  EXPRESSION_OPERATOR_ADD,
  EXPRESSION_OPERATOR_SUBTRACT,
  EXPRESSION_OPERATOR_MULTIPLY,
  EXPRESSION_OPERATOR_DIVIDE,
  EXPRESSION_OPEN_PARENTHESIS,
  EXPRESSION_CLOSE_PARENTHESIS,
  EXPRESSION_TOKEN_NOOP
};

struct Token
{
  std::string token_string;
  EXPRESSION_TOKENS token_type;
  double constant_value;

  Token(std::string token_string = "");
  static std::vector<Token> tokenize(std::string expression_string);
};

struct Expression
{
  std::string expression_string;
  Token token;
  std::pair<Expression *, Expression *> subexpressions;

  Expression(std::string expression_string = "");
  ~Expression();

  static Expression *fromTokens(const std::vector<Token> &tokens);

  double evaluate(const Agent &agent1, const Agent &agent2) const;
};

#endif // !EXPRESSION_H