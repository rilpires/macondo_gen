#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <iostream>

enum VARIABLE_TYPE
{
  VARIABLE_TYPE_INT,
  VARIABLE_TYPE_DOUBLE,
  VARIABLE_TYPE_BOOL,
  VARIABLE_TYPE_STRING,
};

struct Variable
{
  VARIABLE_TYPE type = VARIABLE_TYPE_INT;
  union
  {
    int int_value = 0;
    double double_value;
    bool bool_value;
  };
  std::string string_value;
  static Variable invalid_variable;

  Variable(int value);
  Variable(double value = 0.0);
  Variable(bool value);
  ~Variable();

  Variable &operator=(Variable &value);

  Variable &operator=(int value);

  Variable &operator=(double value);

  Variable &operator=(bool value);

  Variable &operator=(std::string value);

  bool operator==(const Variable &other) const;

  std::string toString() const;

  double toDouble() const;

  std::ostream &operator<<(std::ostream &stream) const;
};

std::ostream &operator<<(std::ostream &stream, const Variable &v);

Variable operator+(const Variable &a, const Variable &b);
Variable operator-(const Variable &a, const Variable &b);
Variable operator/(const Variable &a, const Variable &b);
Variable operator*(const Variable &a, const Variable &b);

#endif