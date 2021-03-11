//
// Created by Alan Huang on 2/27/21.
//

#include "value.h"
#include "object.h"
#include "chunk.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include "common.h"

#include <iostream>
#include <string_view>
#include <cstdint>

namespace alien {

namespace {
  void runtimeError(std::string_view message) {
    std::cerr << message << '\n';
  }
} // namespace

void Vm::push(const Value &value) {
  stack_.push_back(value);
}

Value Vm::pop() {
  Value value = stack_.back();
  stack_.pop_back();
  return value;
}

Value Vm::peek(int depth) {
  assert(stack_.size() > depth);
  int index = stack_.size() - 1 - depth;
  return stack_[index];
}

void Vm::addObj(Obj *obj) {
  objs_.push_back(obj);
}

bool Vm::callValue(const Value &callee, int argCount) {
  if (std::holds_alternative<Obj*>(callee)) {
    Obj* obj = AS_OBJ(callee);
    switch (obj->getType()) {
      case OBJ_FUNCTION: {
        return call(obj->asFunction(), argCount);
      }
      case OBJ_CLASS: {
        auto klass = obj->asClass();
        auto instance = new ObjInstance(klass);
        addObj(instance);
        // we just set the zero slot.(the callee's perspective)
        // it will set the return value to the first slot of itself.
        stack_[stack_.size() - argCount - 1] = instance;
        auto init = klass->findMethod("init");
        if (init) {
          return call(init, argCount);
        } else if (argCount != 0) {
          runtimeError("the number of arguments didn't match.");
          return false;
        }
        return true;
      }
      case OBJ_BOUND_METHOD: {
        auto boundMethod = AS_OBJ(callee)->asBoundMethod();
        stack_[stack_.size() - argCount - 1] = boundMethod->receiver_;
        return call(boundMethod->method_, argCount);
      }
      default:
        runtimeError("can only call functions and classes.");
    }
  }
  return false;
}

bool Vm::call(ObjFunction* callee, int argCount) {
  if (callee->arity() != argCount) {
    runtimeError("the number of arguments and parameters is different.");
    return false;
  }
  CallFrame callFrame(callee, stack_.size() - argCount - 1);
  callFrame.ip = 0;
  callFrames_.push_back(callFrame);
  return true;
}

bool Vm::bindMethod(ObjClass *klass, const std::string &name) {
  auto method = klass->findMethod(name);
  if (!method) {
    return false;
  }
  // no pop because of the garbage collector.
  auto boundMethod = new ObjBoundMethod(method, peek(0));
  addObj(boundMethod);
  pop();
  push(boundMethod);
  return true;
}

InterpretResult Vm::interpret(std::string_view source) {
  Parser parser(source);
  auto program = parser.parse();
//  for (const auto& stmt : program) {
//    stmt->trace(std::cout)callFrame.function->chunk().code()[callFrame.ip++]
  if (parser.hadError()) {
    return INTERPRET_PARSE_ERROR;
  }
  Compiler compiler(*this);
  ObjFunction* script = compiler.compile(program);
  // don't forget this, or some objected will be mistakenly reclaimed .
  // after the first garbage collection, the object's flag will not be flipped.
  // then some objects in the global chunk will be reclaimed.
  addObj(script);
  if (compiler.hadError()) {
    return INTERPRET_COMPILE_ERROR;
  }
  push(script);
  call(script, 0);
  return run();
}

InterpretResult Vm::run() {
  for (;;) {
    collectGarbage();
    auto& callFrame = callFrames_.back();
    auto& chunk = callFrame.function->chunk();

#define READ_BYTE() \
  chunk.code()[callFrame.ip++]
#define READ_CONSTANT() \
  chunk.getConstant(READ_BYTE())
#define BINARY_OP(op) \
  do {  \
    if (!std::holds_alternative<double>(peek(0)) || \
        !std::holds_alternative<double>(peek(1))) { \
      runtimeError("binary operator need its operands to be double."); \
      return INTERPRET_RUNTIME_ERROR; \
    } \
    double b = std::get<double>(pop()); \
    double a = std::get<double>(pop()); \
    push(Value(a op b)); \
  } while (false);

#ifdef TRACE_EXECUTION
  for (const auto& value : stack_) {
    std::cout << '[';
    printValue(value);
    std::cout << ']';
  }
  std::cout << '\n';
  chunk.printConstants();
  std::cout << '\n';
  chunk.disassembleInstruction(callFrame.ip);
#endif
    OpCode instruction = READ_BYTE();
    switch (instruction) {
      case OP_NIL: push(Value()); break;
      case OP_FALSE: push(Value(false)); break;
      case OP_TRUE: push(Value(true)); break;
      case OP_CONSTANT: {
        Value value = READ_CONSTANT();
        push(value);
        break;
      }
      case OP_NOT: {
        push(Value(isFalsy(pop())));
        break;
      }
      case OP_NEGATE: {
        if (!std::holds_alternative<double>(peek(0))) {
          runtimeError("need number after '-'.");
        }
        push(Value(-std::get<double>(pop())));
        break;
      }
      case OP_POP: {
        pop();
        break;
      }
      case OP_PRINT: {
        printValue(pop());
        std::cout << '\n';
        break;
      }
      case OP_LOOP: {
        uint8_t offset = READ_BYTE();
        callFrame.ip -= offset;
        break;
      }
      case OP_JUMP: {
        uint8_t offset = READ_BYTE();
        callFrame.ip += offset;
        break;
      }
      case OP_JUMP_IF_FALSE: {
        uint8_t offset = READ_BYTE();
        if (isFalsy(peek(0))) {
          callFrame.ip += offset;
        }
        break;
      }
      case OP_JUMP_IF_TRUE: {
        uint8_t offset = READ_BYTE();
        if (!isFalsy(peek(0))) {
          callFrame.ip += offset;
        }
        break;
      }
      case OP_ADD: {
        if (std::holds_alternative<double>(peek(0)) &&
            std::holds_alternative<double>(peek(1))) {
          double b = std::get<double>(pop());
          double a = std::get<double>(pop());
          push(Value(a + b));
        } else if (std::holds_alternative<std::string>(peek(0)) &&
                   std::holds_alternative<std::string>(peek(1))) {
          auto r = std::get<std::string>(pop());
          auto l = std::get<std::string>(pop());
          push(Value(l + r));
        } else {
          runtimeError("operator '+' needs two operands in the same type.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;
      case OP_GREATER:  BINARY_OP(>); break;
      case OP_LESS:     BINARY_OP(<); break;
      case OP_EQUAL: {
        auto b = pop();
        auto a = pop();
        push(Value(isEqual(a, b)));
        break;
      }
      case OP_CALL: {
        uint8_t argCount = READ_BYTE();
        Value callee = peek(argCount);
        if (!callValue(callee, argCount)) {
          return INTERPRET_COMPILE_ERROR;
        }
        break;
      }
      case OP_RETURN: {
        auto result = pop();
        stack_.resize(callFrame.stackStart);
        push(result);
        callFrames_.pop_back();
        if (callFrames_.empty()) {
          return INTERPRET_OK;
        }
        break;
      }
      case OP_GET_LOCAL: {
        uint8_t index = READ_BYTE();
        push(stack_[callFrame.stackStart + index]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t index = READ_BYTE();
        // use peek because that we will pop the value
        // after the expression statement.
        // consider `a = b = c;`
        stack_[callFrame.stackStart + index] = peek(0);
        break;
      }
      case OP_GET_GLOBAL: {
        std::string name = std::get<std::string>(READ_CONSTANT());
        if (globals_.find(name) == globals_.end()) {
          runtimeError("Undefined variable.");
          if (name == "main") {
            runtimeError("without main.");
          }
          return INTERPRET_RUNTIME_ERROR;
        }
        push(globals_[name]);
        break;
      }
      case OP_SET_GLOBAL: {
        std::string name = std::get<std::string>(READ_CONSTANT());
        if (globals_.find(name) == globals_.end()) {
          runtimeError("Undefined variable.");
          runtimeError(name);
          return INTERPRET_RUNTIME_ERROR;
        }
        globals_[name] = peek(0);
        break;
      }
      case OP_GET_PROPERTY: {
        std::string name = std::get<std::string>(READ_CONSTANT());
        if (!std::holds_alternative<Obj*>(peek(0)) ||
            AS_OBJ(peek(0))->getType() != OBJ_INSTANCE) {
          runtimeError("only objects have properties.");
          return INTERPRET_RUNTIME_ERROR;
        }
        auto instance = AS_OBJ(peek(0))->asInstance();
        if (instance->exists(name)) {
          pop();
          push(instance->getField(name));
          break;
        }
        if (!bindMethod(instance->getClass(), name)) {
          runtimeError("no such property.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SET_PROPERTY: {
        std::string name = std::get<std::string>(READ_CONSTANT());
        if (!std::holds_alternative<Obj*>(peek(1)) ||
            AS_OBJ(peek(1))->getType() != OBJ_INSTANCE) {
          runtimeError("only objects can set properties.");
          return INTERPRET_RUNTIME_ERROR;
        }
        auto value = pop();
        auto instance = AS_OBJ(pop())->asInstance();
        instance->setField(name, value);
        push(value);
        break;
      }
      case OP_DEFINE_GLOBAL: {
        auto name = std::get<std::string>(READ_CONSTANT());
        globals_[name] = peek(0);
        pop();
        break;
      }
      default:
        assert(false);
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
  }
}

void Vm::collectGarbage(){
  if (objs_.size() >= nextGC) {
#ifdef DEBUG_GC
    std::cout << "collect garbage\n";
#endif
    mark();
    sweep();
#ifdef DEBUG_GC
    std::cout << "end collection\n";
#endif
  }
  nextGC = objs_.size() * 2;
}

void Vm::mark() {
  markRoots();
}

void Vm::sweep() {
  for (auto it = objs_.begin(); it != objs_.end(); ) {
    if ((*it)->isMarked()) {
      (*it)->flip();
      ++it;
    } else {
#ifdef DEBUG_GC
  std::cout << "delete object whose type is ";
  switch ((*it)->getType()) {
    case OBJ_CLASS: {
      std::cout << "class\n";
      break;
    }
    case OBJ_FUNCTION: {
      std::cout << "function\n";
      break;
    }
    case OBJ_INSTANCE: {
      std::cout << "instance\n";
      break;
    }
    case OBJ_BOUND_METHOD: {
      std::cout << "bound method\n";
      break;
    }
  }
#endif
      delete *it;
      it = objs_.erase(it);
    }
  }
}

void Vm::markRoots() {
  for (const auto& item : globals_) {
    if (std::holds_alternative<Obj*>(item.second)) {
      AS_OBJ(item.second)->mark();
    }
  }
  for (auto& value : stack_) {
    if (std::holds_alternative<Obj*>(value)) {
      AS_OBJ(value)->mark();
    }
  }
  // we should mark the frames cause
  // the first slot of the callee
  // in the stack may be replaced with an instance
  for (const auto& frame : callFrames_) {
    frame.function->mark();
  }
}

Vm::~Vm() {
  for (const auto& obj : objs_) {
    delete obj;
  }
}

}