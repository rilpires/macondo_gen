#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "utils.h"

struct Agent;
struct Variable;

// A remainder: 🐧 is a strictly prohibited character of expression strings.
// It is used in the process of tokenizing "glued" expressions like "(self).name"
// turning into "( self ) . name".

enum EXPRESSION_TOKENS : int
{
  EXPRESSION_VARIABLE,
  EXPRESSION_CONSTANT,
  EXPRESSION_OPERATOR_ADD,
  EXPRESSION_OPERATOR_SUBTRACT,
  EXPRESSION_OPERATOR_MULTIPLY,
  EXPRESSION_OPERATOR_DIVIDE,
  EXPRESSION_OPEN_PARENTHESIS,
  EXPRESSION_CLOSE_PARENTHESIS,
  EXPRESSION_POINT_ACCESS,
  EXPRESSION_TOKEN_SELF,
  EXPRESSION_TOKEN_OTHER,
  EXPRESSION_TOKEN_GROUP,

  // functions...
  EXPRESSION_TOKEN_ABS,

  EXPRESSION_TOKEN_NOOP,
  EXPRESSION_TOKEN_INVALID
};

struct Token
{
  std::string token_string;
  EXPRESSION_TOKENS token_type;
  double constant_value;

  Token(double constant_value = 0);
  Token(std::string token_string);
  static std::vector<Token> tokenize(std::string expression_string);
};

struct Expression
{
  std::string expression_string;
  Token token;
  std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>> subexpressions;

  Expression(double constant_value = 0);
  Expression(std::string expression_string);
  ~Expression();

  // gonna private this when class'ing it up
  static Expression *fromTokens(const std::vector<Token> &tokens);

  Variable evaluate(const Agent &agent1, const Agent &agent2) const;

  std::string toString() const;
};

#endif // !EXPRESSION_H