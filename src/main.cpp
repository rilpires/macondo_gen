
#include "utils.h"

int main()
{
  Agent a = Agent(1000, 0);
  auto v = Utils::split("Hello, World!", "l, ");

  Expression e = Expression("1 + 2 * 3");
  std::cout << "Expression result of " << e.expression_string << " : " << e.evaluate(a, a) << std::endl;

  Expression e2 = Expression("(1 + 2)* 3");
  std::cout << "Expression result of " << e2.expression_string << " : " << e2.evaluate(a, a) << std::endl;

  Expression e3 = Expression("(-1 + (2-3))*   3 * (-1)");
  std::cout << "Expression result of " << e3.expression_string << " : " << e3.evaluate(a, a) << std::endl;

  Expression e4 = Expression("0 + 0+0+0+ (1 + (2-3))*   3 * (1)");
  std::cout << "Expression result of " << e4.expression_string << " : " << e4.evaluate(a, a) << std::endl;

  Expression e5 = Expression("-0 + 0+0+0+ (-1 + (2-3))*   3 * (-1)");
  std::cout << "Expression result of " << e5.expression_string << " : " << e5.evaluate(a, a) << std::endl;

  Expression e6 = Expression("2*2 + 3*3");
  std::cout << "Expression result of " << e6.expression_string << " : " << e6.evaluate(a, a) << std::endl;

  return 0;
}