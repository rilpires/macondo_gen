
#include "utils.h"
#include "agent.h"
#include "expression.h"
#include "story.h"

int main()
{
  Story story;
  json j = Utils::loadJSON("storydef.json");
  story.buildFromJSON(j);
  Agent a = story.agents.at(0);
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

  Expression e7 = Expression("0-1-1+1-1+1-0");
  std::cout << "Expression result of " << e7.expression_string << " : " << e7.evaluate(a, a) << std::endl;

  Expression e8 = Expression("-0+((1))-1+1-1-1-0");
  std::cout << "Expression result of " << e8.expression_string << " : " << e8.evaluate(a, a) << std::endl;

  Expression e9 = Expression("wealth + 1");
  std::cout << "Expression result of " << e9.expression_string << " : " << e9.evaluate(a, a) << std::endl;

  Expression e10 = Expression("2*rebel - rebel + rebel*rebel*rebel");
  std::cout << "Expression result of " << e10.expression_string << " : " << e10.evaluate(a, a) << std::endl;

  std::cout << "Bye!" << std::endl;

  return 0;
}