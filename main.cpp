#include "vm.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

enum ExitCode {
  EX_USAGE = 64,
  EX_UNAVAILABLE = 69,
};

std::string readFile(const std::string& file) {
  std::ifstream in_(file);
  if (!in_) {
    std::cerr << "Couldn't open file " << std::quoted(file);
    exit(EX_UNAVAILABLE);
  }
  std::string source(std::istreambuf_iterator<char>(in_.rdbuf()),
                     std::istreambuf_iterator<char>());
  return source;
}

void runScript(const std::string& file) {
  std::string source(readFile(file));
  alien::Vm vm;
  auto result = vm.interpret(source);
  using namespace alien;
  switch (result) {
    case INTERPRET_OK: {
      break;
    }
    case INTERPRET_PARSE_ERROR: {
      std::cerr << "parse error\n";
      break;
    }
    case INTERPRET_COMPILE_ERROR: {
      std::cerr << "compile error\n";
      break;
    }
    case INTERPRET_RUNTIME_ERROR: {
      std::cerr << "runtime error\n";
      break;
    }
    default:
      assert(false);
  }
}

} // namespace

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: alien file";
    return EX_USAGE ;
  }
  runScript(argv[1]);
  return 0;
}