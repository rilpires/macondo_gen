#include "expression.h"

Token::Token(std::string token_string)
{
  this->token_string = token_string;
  if (token_string.size() == 0)
  {
    this->token_type = EXPRESSION_TOKEN_NOOP;
    this->constant_value = 0;
  }
  else if (token_string == "+")
    this->token_type = EXPRESSION_OPERATOR_ADD;
  else if (token_string == "-")
    this->token_type = EXPRESSION_OPERATOR_SUBTRACT;
  else if (token_string == "*")
    this->token_type = EXPRESSION_OPERATOR_MULTIPLY;
  else if (token_string == "/")
    this->token_type = EXPRESSION_OPERATOR_DIVIDE;
  else if (token_string == "(")
    this->token_type = EXPRESSION_OPEN_PARENTHESIS;
  else if (token_string == ")")
    this->token_type = EXPRESSION_CLOSE_PARENTHESIS;
  else if (
      (token_string.find_first_not_of("0123456789.") == std::string::npos))
  {
    this->token_type = EXPRESSION_CONSTANT;
    this->constant_value = std::stod(token_string);
  }
  else if (
      (token_string.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == std::string::npos) &&
      (token_string.find_first_not_of("0123456789") == 0))
  {
    this->token_type = EXPRESSION_VARIABLE;
  }
  else
  {
    std::cout << "Unexpected token : " << token_string << std::endl;
    this->token_type = EXPRESSION_TOKEN_INVALID;
  }
};

Expression::Expression(std::string expression_string)
{
  this->expression_string = expression_string;
  this->token.token_type = EXPRESSION_TOKEN_NOOP;
  if (expression_string.length() > 0)
  {
    subexpressions.first = std::shared_ptr<Expression>(fromTokens(Token::tokenize(expression_string)));
  }
}

Expression::~Expression()
{
  // this is my first time trusting shared_ptr... kinda scary!
  // if (subexpressions.first.get() != NULL)
  //   delete subexpressions.first.get();
  // if (subexpressions.second.get() != NULL)
  //   delete subexpressions.second.get();
}

std::vector<Token> Token::tokenize(std::string expression_string)
{
  // we should split glued tokens like "1+2" into "1", "+", "2"
  // I'll use a cheap trick to do this: insert spaces before and after operators
  // Later gotta be careful with parenthesis though (function call)

  Utils::replaceAll(expression_string, "+", " + ");
  Utils::replaceAll(expression_string, "-", " - ");
  Utils::replaceAll(expression_string, "*", " * ");
  Utils::replaceAll(expression_string, "/", " / ");
  Utils::replaceAll(expression_string, "(", " ( ");
  Utils::replaceAll(expression_string, ")", " ) ");

  std::vector<Token> tokens;
  std::vector<std::string> token_strings = Utils::split(expression_string, "\t\n ");
  for (std::string token_string : token_strings)
  {
    if (token_string.length() > 0)
    {
      tokens.push_back(Token(token_string));
    }
  }
  return tokens;
}

Expression *Expression::fromTokens(const std::vector<Token> &tokens)
{

  std::string expression_string = "";
  for (Token token : tokens)
    expression_string += token.token_string + " ";

  // std::cout << "Currently evaluating:" << expression_string << std::endl;

  int parenthesis_count = 0;
  bool sequence_start = true;

  // Accounting for the case of a negative number or a redundant initial "+""
  // Inserting a zero at the start of sequence
  for (int i = 0; i < tokens.size(); i++)
  {
    const Token &token = tokens[i];
    if (token.token_type == EXPRESSION_CLOSE_PARENTHESIS)
    {
      parenthesis_count++;
      sequence_start = false;
    }
    else if (token.token_type == EXPRESSION_OPEN_PARENTHESIS)
    {
      parenthesis_count--;
      sequence_start = true;
    }
    else if (sequence_start && (parenthesis_count == 0) && (token.token_type == EXPRESSION_OPERATOR_ADD || token.token_type == EXPRESSION_OPERATOR_SUBTRACT))
    {
      std::vector<Token> fixed_tokens = tokens;
      fixed_tokens.insert(fixed_tokens.begin() + i, Token("0"));
      return fromTokens(fixed_tokens);
    }
    else
    {
      sequence_start = false;
    }
  }

  parenthesis_count = 0;

  // Least precedent operators: addition and subtraction
  // We gotta start from the end, because we want to evaluate the first operators first
  for (int i = tokens.size() - 1; i >= 0; i--)
  {
    const Token &token = tokens[i];
    if (token.token_type == EXPRESSION_CLOSE_PARENTHESIS)
    {
      parenthesis_count++;
    }
    else if (token.token_type == EXPRESSION_OPEN_PARENTHESIS)
    {
      parenthesis_count--;
    }
    else if (parenthesis_count == 0 && (token.token_type == EXPRESSION_OPERATOR_ADD || token.token_type == EXPRESSION_OPERATOR_SUBTRACT))
    {
      Expression *expression = new Expression();
      expression->token = tokens[i];
      expression->subexpressions.first = std::shared_ptr<Expression>(fromTokens(std::vector<Token>(tokens.begin(), tokens.begin() + i)));
      expression->subexpressions.second = std::shared_ptr<Expression>(fromTokens(std::vector<Token>(tokens.begin() + i + 1, tokens.end())));
      expression->expression_string = expression_string;
      return expression;
    }
  }

  parenthesis_count = 0;

  // Most precedent operators: multiplication and division
  // We gotta start from the end, because we want to evaluate the first operators first
  for (int i = tokens.size() - 1; i >= 0; i--)
  {
    const Token &token = tokens[i];
    if (token.token_type == EXPRESSION_CLOSE_PARENTHESIS)
      parenthesis_count++;
    else if (token.token_type == EXPRESSION_OPEN_PARENTHESIS)
      parenthesis_count--;
    else if (parenthesis_count == 0 && (token.token_type == EXPRESSION_OPERATOR_MULTIPLY || token.token_type == EXPRESSION_OPERATOR_DIVIDE))
    {
      Expression *expression = new Expression();
      expression->token = tokens[i];
      expression->subexpressions.first = std::shared_ptr<Expression>(fromTokens(std::vector<Token>(tokens.begin(), tokens.begin() + i)));
      expression->subexpressions.second = std::shared_ptr<Expression>(fromTokens(std::vector<Token>(tokens.begin() + i + 1, tokens.end())));
      expression->expression_string = expression_string;
      return expression;
    }
  }

  // No operators found, so this is a constant or variable or parenthesis
  if (tokens.size() == 1)
  {
    const Token &token = tokens[0];
    if (token.token_type == EXPRESSION_CONSTANT || token.token_type == EXPRESSION_VARIABLE)
    {
      Expression *expression = new Expression();
      expression->expression_string = expression_string;
      expression->token = tokens[0];
      return expression;
    }
    else
    {
      std::cout << "Unexpected token : " << token.token_string << std::endl;
    }
  }
  else if ((tokens.size() >= 3) && (tokens[0].token_type == EXPRESSION_OPEN_PARENTHESIS) && (tokens[tokens.size() - 1].token_type == EXPRESSION_CLOSE_PARENTHESIS))
  {
    return fromTokens(std::vector<Token>(tokens.begin() + 1, tokens.end() - 1));
  }
  else
  {
    std::cout << "Unexpected tokens : ";
    for (Token token : tokens)
      std::cout << token.token_string << " ";
    std::cout << std::endl;
  }

  return NULL;
};

double Expression::evaluate(const Agent &agent1, const Agent &agent2) const
{
  double ret = 0;
  switch (token.token_type)
  {
  case EXPRESSION_VARIABLE:
  {
    ret = agent1[token.token_string].toDouble();
    break;
  }
  case EXPRESSION_CONSTANT:
  {
    ret = token.constant_value;
    break;
  }
  case EXPRESSION_OPERATOR_ADD:
  {
    ret = subexpressions.first->evaluate(agent1, agent2) + subexpressions.second->evaluate(agent1, agent2);
    break;
  }
  case EXPRESSION_OPERATOR_SUBTRACT:
  {
    ret = subexpressions.first->evaluate(agent1, agent2) - subexpressions.second->evaluate(agent1, agent2);
    break;
  }
  case EXPRESSION_OPERATOR_MULTIPLY:
  {
    ret = subexpressions.first->evaluate(agent1, agent2) * subexpressions.second->evaluate(agent1, agent2);
    break;
  }
  case EXPRESSION_OPERATOR_DIVIDE:
  {
    ret = subexpressions.first->evaluate(agent1, agent2) / subexpressions.second->evaluate(agent1, agent2);
    break;
  }
  case EXPRESSION_TOKEN_NOOP:
    ret = subexpressions.first->evaluate(agent1, agent2);
    break;
  };

#ifdef DEBUG
  // std::cout << "Evaluating " << expression_string << " : " << ret << std::endl;
#endif

  return ret;
}