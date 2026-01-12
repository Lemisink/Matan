#include <cstdlib>
#include <iostream>

#include "Expression.h"

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: expr_demo \"<expr>\" <x>\n";
    return 1;
  }
  try {
    matan::Expression expr(argv[1]);
    double x = std::strtod(argv[2], nullptr);
    std::cout << expr.eval(x) << "\n";
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }
  return 0;
}
