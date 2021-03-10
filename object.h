//
// Created by Alan Huang on 2/25/21.
//

#ifndef ALIEN_OBJECT_H
#define ALIEN_OBJECT_H

#include "value.h"
#include "chunk.h"
#include "common.h"

#include <string>
#include <ostream>
#include <unordered_map>

namespace alien {

enum ObjType {
  OBJ_CLASS,
  OBJ_FUNCTION,
  OBJ_INSTANCE,
  OBJ_BOUND_METHOD,
};

class ObjFunction;
class ObjClass;
class ObjInstance;
class ObjBoundMethod;
// runtime objects
class Obj {
public:
  explicit Obj(ObjType type)
  : type_(type), isMarked_(false) {}
  ObjType getType() const { return type_; }
  bool    isMarked() const { return isMarked_; }
  virtual void mark() {
    isMarked_ = true;
  }
  void flip() { isMarked_ = !isMarked_; }
  virtual void print(std::ostream& os) = 0;
  virtual ~Obj() = default;
public:
  virtual ObjFunction* asFunction() { return nullptr; }
  virtual ObjClass*    asClass() { return nullptr; }
  virtual ObjInstance* asInstance() { return nullptr; }
  virtual ObjBoundMethod* asBoundMethod() { return nullptr; }
private:
  ObjType type_;
  bool isMarked_;
};

class ObjFunction : public Obj {
public:
  explicit ObjFunction(std::string name, Chunk chunk, int arity)
  : Obj(OBJ_FUNCTION), name_(std::move(name)),
    chunk_(std::move(chunk)), arity_(arity) {}
  ~ObjFunction() override = default;
  void mark() override {
    if (isMarked()) {
      return;
    }
    Obj::mark();
#ifdef DEBUG_GC
    std::cout << "mark function " << name_ << "\n";
#endif
    // we have global functions in the global chunk.
    for (const auto& value : chunk_.constants()) {
      if (std::holds_alternative<Obj*>(value)) {
        AS_OBJ(value)->mark();
      }
    }
  }
  void print(std::ostream& os) override {
    os << "[func] " << name_;
  }
  int          arity() { return arity_; }
  Chunk&       chunk() { return chunk_; }
  ObjFunction* asFunction() override { return this; }
private:
  std::string name_;
  Chunk chunk_;
  int arity_;
};


class ObjClass : public Obj {
public:
  explicit ObjClass(std::string name)
  : Obj(OBJ_CLASS), name_(std::move((name))) {}
  ~ObjClass() override = default;
  ObjFunction* findMethod(const std::string& name) {
    if (methods_.find(name) != methods_.end()) {
      return methods_[name];
    }
    return nullptr;
  }
  void addMethod(const std::string& name, ObjFunction* func) {
    // TODO: check duplication
    methods_[name] = func;
  }
  void mark() override {
    if (isMarked()) {
      return;
    }
    Obj::mark();
#ifdef DEBUG_GC
    std::cout << "mark class " << name_ << "\n";
#endif
    for (const auto& item : methods_) {
      item.second->mark();
    }
  }
  void print(std::ostream& os) override {
    os << "[class] " << name_;
  }
  ObjClass* asClass() override { return this; }
private:
  std::string name_;
  std::unordered_map<std::string, ObjFunction*> methods_;
};

class ObjInstance : public Obj {
public:
  explicit ObjInstance(ObjClass* klass)
  : Obj(OBJ_INSTANCE), klass_(klass) {}
  ~ObjInstance() override = default;
  ObjInstance* asInstance() override { return this; }
  ObjClass* getClass() { return klass_; }
  bool exists(const std::string& name) {
    return fields_.find(name) != fields_.end();
  }
  Value getField(const std::string& name) {
    if (exists(name)) return fields_[name];
    return Value();
  }
  void setField(const std::string& name, const Value& value) {
    fields_[name] = value;
  }
  void mark() override {
    if (isMarked()) {
      return;
    }
    Obj::mark();
#ifdef DEBUG_GC
    std::cout << "mark instance\n";
#endif
    klass_->mark();
    for (const auto& item : fields_) {
      if (std::holds_alternative<Obj*>(item.second)) {
        AS_OBJ(item.second)->mark();
      }
    }
  }
  void print(std::ostream& os) override {
    os << "[instance] -> ";
    klass_->print(os);
  }

private:
  ObjClass* klass_;
  std::unordered_map<std::string, Value> fields_;
};

class ObjBoundMethod : public Obj {
public:
  ObjBoundMethod(ObjFunction* method, const Value& value)
  : Obj(OBJ_BOUND_METHOD), method_(method), receiver_(value) {}
  ~ObjBoundMethod() override = default;
  ObjBoundMethod* asBoundMethod() override { return this; }
  void mark() override {
    if (isMarked()) {
      return;
    }
    Obj::mark();
    method_->mark();
#ifdef DEBUG_GC
    std::cout << "mark bound method\n";
#endif
    // it absolutely holds the Obj.
    if (std::holds_alternative<Obj*>(receiver_)) {
      AS_OBJ(receiver_)->mark();
    }
  }
  void print(std::ostream& os) override {
    os << "[method] -> ";
    method_->print(os);
  }
  ObjFunction* method_;
  Value receiver_;
};

}






#endif //ALIEN_OBJECT_H
