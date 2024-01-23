#include "expression.h"
#include "agent.h"
#include "variable.h"

Token::Token(double constant_value)
{
  this->token_type = EXPRESSION_CONSTANT;
  this->constant_value = constant_value;
}
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
  else if (token_string == ".")
    this->token_type = EXPRESSION_POINT_ACCESS;
  else if (token_string == "self")
    this->token_type = EXPRESSION_TOKEN_SELF;
  else if (token_string == "other")
    this->token_type = EXPRESSION_TOKEN_OTHER;
  else if (token_string == "abs")
    this->token_type = EXPRESSION_TOKEN_ABS;
  else if (
      (token_string.find_first_not_of("0123456789.") == std::string::npos))
  {
    this->token_type = EXPRESSION_CONSTANT;
    this->constant_value = std::stod(token_string);
  }
  else if (token_string.find_first_of(".") != std::string::npos)
  {
    this->token_type = EXPRESSION_TOKEN_GROUP;
    this->token_string = Utils::replaceAll(token_string, ".", "🐧.🐧");
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

Expression::Expression(double constant_value) : token(Token(constant_value)) {}
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

  expression_string = Utils::replaceAll(expression_string, "🐧", " ");
  expression_string = Utils::replaceAll(expression_string, "+", " + ");
  expression_string = Utils::replaceAll(expression_string, "-", " - ");
  expression_string = Utils::replaceAll(expression_string, "*", " * ");
  expression_string = Utils::replaceAll(expression_string, "/", " / ");
  expression_string = Utils::replaceAll(expression_string, "(", " ( ");
  expression_string = Utils::replaceAll(expression_string, ")", " ) ");

  std::vector<Token> tokens;
  std::vector<std::string> token_strings = Utils::split(expression_string, "\t\n ");
  for (std::string token_string : token_strings)
  {
    if (token_string.length() > 0)
    {
      Token token(token_string);
      if (token.token_type == EXPRESSION_TOKEN_GROUP)
      {
        std::vector<Token> subtoken_strings = Token::tokenize(token.token_string);
        tokens.insert(tokens.end(), subtoken_strings.begin(), subtoken_strings.end());
      }
      else
      {
        tokens.push_back(token);
      }
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

  // Least precedente operators come first, since we want them in the bottom of the expression tree
  // 1 - Lets first iterate over ADD and SUBTRACT operators (+ and -)
  // 2 - Then we'll iterate over MULTIPLY and DIVIDE operators (* and /)
  // 3 - Functions will come next
  // 4 - Then we'll iterate over POINT ACCESS operators (.)
  // Also, since these operators are left-associative, we'll start from the end of the expression

  // ADD and SUBTRACT operators
  parenthesis_count = 0;
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

  // MULTIPLY and DIVIDE operators
  parenthesis_count = 0;
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

  // Functions
  parenthesis_count = 0;
  for (int i = tokens.size() - 1; i >= 0; i--)
  {
    const Token &token = tokens[i];
    const Token &next_token = tokens[i + 1];

    if (token.token_type == EXPRESSION_CLOSE_PARENTHESIS)
      parenthesis_count++;
    else if (token.token_type == EXPRESSION_OPEN_PARENTHESIS)
      parenthesis_count--;
    else if ((parenthesis_count == 0) && next_token.token_type == EXPRESSION_OPEN_PARENTHESIS)
    {
      if (token.token_type == EXPRESSION_TOKEN_ABS || token.token_type == EXPRESSION_VARIABLE)
      {
        Expression *expression = new Expression();
        expression->token = tokens[i];
        expression->subexpressions.first = std::shared_ptr<Expression>(fromTokens(std::vector<Token>(tokens.begin() + i + 2, tokens.end() - 1)));
        expression->expression_string = expression_string;
        return expression;
      }
      else
      {
        std::cout << "Unexpected token used as a function : " << token.token_string << std::endl;
        return NULL;
      }
    }
  }

  // Point access operators
  for (int i = tokens.size() - 1; i >= 0; i--)
  {
    const Token &token = tokens[i];
    if (token.token_type == EXPRESSION_CLOSE_PARENTHESIS)
      parenthesis_count++;
    else if (token.token_type == EXPRESSION_OPEN_PARENTHESIS)
      parenthesis_count--;
    else if (parenthesis_count == 0 && token.token_type == EXPRESSION_POINT_ACCESS)
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
    if (
        token.token_type == EXPRESSION_CONSTANT ||
        token.token_type == EXPRESSION_VARIABLE ||
        token.token_type == EXPRESSION_TOKEN_SELF ||
        token.token_type == EXPRESSION_TOKEN_OTHER)
    {
      Expression *expression = new Expression();
      expression->expression_string = expression_string;
      expression->token = tokens[0];
      return expression;
    }
    else
    {
      std::cout << "Unexpected single token : " << token.token_string << std::endl;
    }
  }
  else if ((tokens.size() >= 3) && (tokens[0].token_type == EXPRESSION_OPEN_PARENTHESIS) && (tokens[tokens.size() - 1].token_type == EXPRESSION_CLOSE_PARENTHESIS))
  {
    // We should be sure that these open and close parenthesis are matching. If not, we will have an error in the next recursions
    return fromTokens(std::vector<Token>(tokens.begin() + 1, tokens.end() - 1));
  }
  else
  {
    std::cout << "Unexpected tokens (no valid rule untill here): ";
    for (Token token : tokens)
      std::cout << token.token_string << " ";
    std::cout << std::endl;
  }

  return NULL;
};

Variable Expression::evaluate(const Agent &agent) const
{
#ifdef DEBUG
  // std::cout << "Evaluating " << expression_string << " : " << ret << std::endl;
#endif

  switch (token.token_type)
  {
  case EXPRESSION_VARIABLE:
  {
    // In the future this could be a dynamic function
    return agent.getVariable(token.token_string);
  }
  case EXPRESSION_CONSTANT:
  {
    return Variable(token.constant_value);
  }
  case EXPRESSION_POINT_ACCESS:
  {
    auto expr1 = subexpressions.first;
    auto expr2 = subexpressions.second;
    if (expr1->token.token_type == EXPRESSION_TOKEN_SELF)
    {
      return agent.getVariable(expr2->token.token_string);
    }
    else if (expr1->token.token_type == EXPRESSION_TOKEN_OTHER)
    {
      return agent.getOtherAgent().getVariable(expr2->token.token_string);
    }
    else
    {
      std::cout << "Unexpected token : " << expr1->token.token_string << std::endl;
    }
  }
  case EXPRESSION_TOKEN_ABS:
  {
    auto arg1 = subexpressions.first->evaluate(agent);
    return Variable(abs(arg1.toDouble()));
  }
  case EXPRESSION_OPERATOR_ADD:
  {
    auto arg1 = subexpressions.first->evaluate(agent);
    auto arg2 = subexpressions.second->evaluate(agent);
    return arg1 + arg2;
  }
  case EXPRESSION_OPERATOR_SUBTRACT:
  {
    auto arg1 = subexpressions.first->evaluate(agent);
    auto arg2 = subexpressions.second->evaluate(agent);
    return arg1 - arg2;
  }
  case EXPRESSION_OPERATOR_MULTIPLY:
  {
    auto arg1 = subexpressions.first->evaluate(agent);
    auto arg2 = subexpressions.second->evaluate(agent);
    return arg1 * arg2;
  }
  case EXPRESSION_OPERATOR_DIVIDE:
  {
    auto arg1 = subexpressions.first->evaluate(agent);
    auto arg2 = subexpressions.second->evaluate(agent);
    return arg1 / arg2;
  }
  case EXPRESSION_TOKEN_NOOP:
    return subexpressions.first->evaluate(agent);
  };

  return Variable(0);
}