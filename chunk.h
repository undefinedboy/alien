//
// Created by Alan Huang on 2/27/21.
//

#ifndef ALIEN_CHUNK_H
#define ALIEN_CHUNK_H

#include "value.h"

#include <vector>
#include <ostream>

#include <cstdint>

namespace alien {

enum OpCode : uint8_t {
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_CONSTANT,
  OP_PRINT,

  OP_EQUAL,
  OP_GREATER,
  OP_LESS,

  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,

  OP_NOT,
  OP_NEGATE,
  OP_CALL,
  OP_RETURN,

  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,

  OP_DEFINE_GLOBAL,

  OP_LOOP,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_JUMP_IF_TRUE,

  OP_POP,
};

class Chunk {
public:
  void write(OpCode byte) { code_.push_back(byte); }
  Value getConstant(int index) { return constants_[index]; }
  int addConstant(const Value& value);
  void disassemble(std::ostream& os = std::cout);
  void disassembleInstruction(int i, std::ostream& os = std::cout);
  void printConstants(std::ostream& os = std::cout);
  std::vector<OpCode>& code() { return code_; }
  // cause we can't include the object.h
  std::vector<Value>& constants() { return constants_; }
private:
  std::vector<OpCode> code_;
  // which line does this bytecode
  // belongs to in source code.
  std::vector<Value> constants_;
};

}



#endif //ALIEN_CHUNK_H
