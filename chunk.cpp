//
// Created by Alan Huang on 2/27/21.
//

#include "chunk.h"

#include <ostream>

namespace alien {

int Chunk::addConstant(const Value &value) {
  constants_.push_back(value);
  return constants_.size() - 1;
}

void Chunk::disassembleInstruction(int i, std::ostream &os) {
  OpCode code = code_[i];
    switch (code) {
      case OP_NIL: {
        os << "OP_NIL\n";
        break;
      }
      case OP_TRUE: {
        os << "OP_TRUE\n";
        break;
      }
      case OP_FALSE: {
        os << "OP_FALSE\n";
        break;
      }
      case OP_CONSTANT: {
        int index = code_[++i];
        os << "OP_CONSTANT " << index << "(";
        printValue(constants_[index], os);
        os << ")\n";
        break;
      }
      case OP_PRINT: {
        os << "OP_PRINT\n";
        break;
      }
      case OP_EQUAL: {
        os << "OP_EQUAL\n";
        break;
      }
      case OP_GREATER: {
        os << "OP_GREATER\n";
        break;
      }
      case OP_LESS: {
        os << "OP_LESS\n";
        break;
      }
      case OP_ADD: {
        os << "OP_ADD\n";
        break;
      }
      case OP_SUBTRACT: {
        os << "OP_SUBTRACT\n";
        break;
      }
      case OP_MULTIPLY: {
        os << "OP_MULTIPLY\n";
        break;
      }
      case OP_DIVIDE: {
        os << "OP_DIVIDE\n";
        break;
      }
      case OP_NOT: {
        os << "OP_NOT\n";
        break;
      }
      case OP_NEGATE: {
        os << "OP_NEGATE\n";
        break;
      }
      case OP_CALL: {
        os << "OP_CALL " << code_[++i] << '\n';
        break;
      }
      case OP_RETURN: {
        os << "OP_RETURN\n";
        break;
      }
      case OP_GET_LOCAL: {
        os << "OP_GET_LOCAL " << code_[++i] << '\n';
        break;
      }
      case OP_SET_LOCAL: {
        os << "OP_SET_LOCAL " << code_[++i] << '\n';
        break;
      }
      case OP_GET_GLOBAL: {
        int index = code_[++i];
        os << "OP_GET_GLOBAL " << index << "(";
        printValue(constants_[index]);
        os << ")\n";
        break;
      }
      case OP_SET_GLOBAL: {
        int index = code_[++i];
        os << "OP_SET_GLOBAL " << index << "(";
        printValue(constants_[index]);
        os << ")\n";
        break;
      }
      case OP_GET_PROPERTY: {
        int index = code_[++i];
        os << "OP_GET_PROPERTY " << index << "(";
        printValue(constants_[index]);
        os << ")\n";
        break;
      }
      case OP_SET_PROPERTY: {
        int index = code_[++i];
        os << "OP_SET_PROPERTY " << index << "(";
        printValue(constants_[index]);
        os << ")\n";
        break;
      }
      case OP_DEFINE_GLOBAL: {
        int index = code_[++i];
        os << "OP_DEFINE_GLOBAL " << index << "(";
        printValue(constants_[index]);
        os << ")\n";
        break;
      }
      case OP_LOOP: {
        os << "OP_LOOP " << code_[++i] << '\n';
        break;
      }
      case OP_JUMP: {
        os << "OP_JUMP " << code_[++i] << '\n';
        break;
      }
      case OP_JUMP_IF_FALSE: {
        os << "OP_JUMP_IF_FALSE " << code_[++i] << '\n';
        break;
      }
      case OP_JUMP_IF_TRUE: {
        os << "OP_JUMP_IF_TRUE " << code_[++i] << '\n';
        break;
      }
      case OP_POP: {
        os << "OP_POP\n";
        break;
      }
    }
}

void Chunk::disassemble(std::ostream &os) {
  for (int i = 0; i < code_.size(); i++) {
    disassembleInstruction(i, os);
  }
}

void Chunk::printConstants(std::ostream &os) {
  for (int i = 0; i < constants_.size(); i++) {
    os << '[' << i << ' ';
    printValue(constants_[i], os);
    os << "]";
  }
}


}