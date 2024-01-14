#include "variable.h"

Variable Variable::invalid_variable = 0.0;

Variable::Variable(int value) : type(VARIABLE_TYPE_INT), int_value(value) {}
Variable::Variable(double value) : type(VARIABLE_TYPE_DOUBLE), double_value(value) {}
Variable::Variable(bool value) : type(VARIABLE_TYPE_BOOL), bool_value(value) {}
Variable::~Variable() {}

Variable &Variable::operator=(Variable &value)
{
  type = value.type;
  switch (type)
  {
  case VARIABLE_TYPE_INT:
    int_value = value.int_value;
    break;
  case VARIABLE_TYPE_DOUBLE:
    double_value = value.double_value;
    break;
  case VARIABLE_TYPE_BOOL:
    bool_value = value.bool_value;
    break;
  case VARIABLE_TYPE_STRING:
    string_value = value.string_value;
    break;
  }
  return *this;
}

Variable &Variable::operator=(int value)
{
  type = VARIABLE_TYPE_INT;
  int_value = value;
  return *this;
}

Variable &Variable::operator=(double value)
{
  type = VARIABLE_TYPE_DOUBLE;
  double_value = value;
  return *this;
}

Variable &Variable::operator=(bool value)
{
  type = VARIABLE_TYPE_BOOL;
  bool_value = value;
  return *this;
}

Variable &Variable::operator=(std::string value)
{
  type = VARIABLE_TYPE_STRING;
  string_value = value;
  return *this;
}

bool Variable::operator==(const Variable &other) const
{
  if (type != other.type)
    return false;
  switch (type)
  {
  case VARIABLE_TYPE_INT:
    return int_value == other.int_value;
  case VARIABLE_TYPE_DOUBLE:
    return double_value == other.double_value;
  case VARIABLE_TYPE_BOOL:
    return bool_value == other.bool_value;
  case VARIABLE_TYPE_STRING:
    return string_value == other.string_value;
  }
  return false;
}

std::string Variable::toString() const
{
  switch (type)
  {
  case VARIABLE_TYPE_INT:
    return std::to_string(int_value);
  case VARIABLE_TYPE_DOUBLE:
    // we only to print only 2 decimals
    return std::to_string(double_value);
  case VARIABLE_TYPE_BOOL:
    return std::to_string(bool_value);
  case VARIABLE_TYPE_STRING:
    return string_value;
  }
  return "";
}

double Variable::toDouble() const
{
  switch (type)
  {
  case VARIABLE_TYPE_INT:
    return (double)int_value;
  case VARIABLE_TYPE_DOUBLE:
    return double_value;
  case VARIABLE_TYPE_BOOL:
    return (double)bool_value;
  }
  return 0;
}

std::ostream &Variable::operator<<(std::ostream &stream) const
{
  stream << toString();
  return stream;
}

Variable operator+(const Variable &a, const Variable &b)
{
  switch (a.type)
  {
  case VARIABLE_TYPE_INT:
  case VARIABLE_TYPE_DOUBLE:
    return Variable(a.toDouble() + b.toDouble());
  }
  return Variable(0);
}
Variable operator-(const Variable &a, const Variable &b)
{
  switch (a.type)
  {
  case VARIABLE_TYPE_INT:
  case VARIABLE_TYPE_DOUBLE:
    return Variable(a.toDouble() - b.toDouble());
  }
  return Variable();
}
Variable operator*(const Variable &a, const Variable &b)
{
  switch (a.type)
  {
  case VARIABLE_TYPE_INT:
  case VARIABLE_TYPE_DOUBLE:
    return Variable(a.toDouble() * b.toDouble());
  }
  return Variable();
}
Variable operator/(const Variable &a, const Variable &b)
{
  switch (a.type)
  {
  case VARIABLE_TYPE_INT:
  case VARIABLE_TYPE_DOUBLE:
    if (b.toDouble() == 0)
      return Variable(1.79769e+308);
    else
      return Variable(a.toDouble() / b.toDouble());
  case VARIABLE_TYPE_BOOL:
    return Variable(a.bool_value / b.bool_value);
  }
  return Variable();
}

std::ostream &operator<<(std::ostream &stream, const Variable &v)
{
  return v.operator<<(stream);
}