//
// Created by Alan Huang on 3/2/21.
//

#include "value.h"
#include "object.h"

#include <variant>

namespace alien {

struct OutputVisitor {
  explicit OutputVisitor(std::ostream& os)
  : os_(os) {}
  void operator()(const double d) const {
    os_ << d;
  }
  void operator()(const bool b) const {
    os_ << (b ? "true" : "false");
  }
  void operator()(const std::monostate n) const {
    os_ << "nil";
  }
  void operator()(const std::string& s) const {
    os_ << s;
  }
  void operator()(Obj* o) const {
    o->print(os_);
  }
  std::ostream& os_;
};

struct FalsinessVisitor {
  bool operator()(const double d) const { return false; }
  bool operator()(const bool b) const { return !b; }
  bool operator()(const std::monostate n) const { return true; }
  bool operator()(const std::string& s) const { return false; }
};

void printValue(const Value& value, std::ostream& os) {
  std::visit(OutputVisitor(os), value);
}

bool isFalsy(const Value& value) {
  return std::visit(FalsinessVisitor(), value);
}

bool isEqual(const Value& lhs, const Value& rhs) {
  return lhs == rhs;
}

}