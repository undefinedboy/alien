//
// Created by Alan Huang on 2/28/21.
//

#ifndef ALIEN_VALUE_H
#define ALIEN_VALUE_H

#include <iostream>
#include <string>
#include <variant>

namespace alien {

class Obj;
// we use std::string here for convenience.(std::string_view doesn't fit in here)
using Value = std::variant<std::monostate, bool, double, std::string, Obj*>;

#define AS_OBJ(value) std::get<Obj*>(value)

void printValue(const Value& value, std::ostream& os = std::cout);
bool isFalsy(const Value& value);
bool isEqual(const Value& lhs, const Value& rhs);

}

#endif //ALIEN_VALUE_H
