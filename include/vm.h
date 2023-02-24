//
// Created by Alan Huang on 2/27/21.
//

#ifndef ALIEN_VM_H
#define ALIEN_VM_H

#include <value.h>
#include <object.h>

#include <string>
#include <list>
#include <unordered_map>

namespace alien {

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_PARSE_ERROR,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
};

struct CallFrame {
  CallFrame(ObjFunction* function, int stackStart)
  : function(function), stackStart(stackStart), ip(0) {}
  ObjFunction* function;
  int stackStart;
  int ip;
};

class Vm {
public:
  InterpretResult interpret(std::string_view source);
  void addObj(Obj* obj);
  ~Vm();
private:
  InterpretResult run();
  bool callValue(const Value& callee, int argCount);
  bool call(ObjFunction* callee, int argCount);
  bool bindMethod(ObjClass* klass, const std::string& name);

private:
  void collectGarbage();
  void mark();
  void sweep();
  void markRoots();

private:
  void  push(const Value& value);
  Value pop();
  Value peek(int depth);

private:
  int nextGC = 50;
  // global definitions.
  std::unordered_map<std::string, Value> globals_;
  // heap-allocated objects.
  std::list<Obj*> objs_;
  // runtime stack.
  std::vector<Value> stack_;
  std::vector<CallFrame> callFrames_;
};

}



#endif //ALIEN_VM_H
