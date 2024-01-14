#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>

enum PARAMETER_TYPE
{
  PARAMETER_TYPE_INT,
  PARAMETER_TYPE_DOUBLE,
  PARAMETER_TYPE_BOOL
};

struct Parameter
{
  PARAMETER_TYPE type = PARAMETER_TYPE_INT;
  union
  {
    int int_value = 0;
    double double_value;
    bool bool_value;
  };

  Parameter(int value) : type(PARAMETER_TYPE_INT), int_value(value) {}
  Parameter(double value = 0.0) : type(PARAMETER_TYPE_DOUBLE), double_value(value) {}
  Parameter(bool value) : type(PARAMETER_TYPE_BOOL), bool_value(value) {}
  ~Parameter() {}

  Parameter &operator=(Parameter &value)
  {
    type = value.type;
    switch (type)
    {
    case PARAMETER_TYPE_INT:
      int_value = value.int_value;
      break;
    case PARAMETER_TYPE_DOUBLE:
      double_value = value.double_value;
      break;
    case PARAMETER_TYPE_BOOL:
      bool_value = value.bool_value;
      break;
    }
    return *this;
  }

  std::string toString()
  {
    switch (type)
    {
    case PARAMETER_TYPE_INT:
      return std::to_string(int_value);
    case PARAMETER_TYPE_DOUBLE:
      return std::to_string(double_value);
    case PARAMETER_TYPE_BOOL:
      return std::to_string(bool_value);
    }
    return "";
  }

  double toDouble() const
  {
    switch (type)
    {
    case PARAMETER_TYPE_INT:
      return (double)int_value;
    case PARAMETER_TYPE_DOUBLE:
      return double_value;
    case PARAMETER_TYPE_BOOL:
      return (double)bool_value;
    }
    return 0;
  }
};

#endif