#include <iostream>
#include <string>
#include <stdexcept>
#include "context.h"

// class Operation;
// typedef std::vector<Operation*> Code;

Value *Context::SearchVariable(VariableId id) {
    auto search = lookup.find(id);
    if (search != lookup.end()) {
        return search->second;
    } 
    if (this->parent) {
        return this->parent->SearchVariable(id);
    } else {
        return NULL;
    }
}

Value *Context::CreateVariable(VariableId id) {
    Value* val = new Value();
    lookup[id] = val;
    return val;
}

Value *Context::GetVariable(VariableId id) {
    Value* val = this->SearchVariable(id);
    if (val) return val;
    return CreateVariable(id);
}

Context::Context(Context *parent, Address position, Value* arg): Context(*(parent->syntax), position, arg) {
     this->parent = parent;
}

Context::Context(SyntaxAnalys &syntax, Address position, Value* arg) {
    this->parent = NULL;
    this->syntax = &syntax;
    this->pointer = position;
    this->result = new Value(DefaultValueType(0));                                // Неявное определение 0 в языку тут
    CreateVariable(syntax.arguments_id)->Define(arg);                             // Аргумкнты
}

Value *Context::Pop() {
    if (!stack.size()) 
        return new Value();
    Value *v = stack.top();
    stack.pop();
    return v;
}

Value *Context::Top() {
    if (!stack.size()) 
        return new Value();
    return stack.top();
}

void Context::Push(Value *v) {
    if (!v) return;
    stack.push(v);
}

void Context::Jump(Address a) {
    pointer = a;
}

void Context::Result(Value *result) {
    this->result->Define(result);
}

Value *Context::Run() {
    ReturnCode ret = ReturnCode::None;
    while (pointer < syntax->code.size()) {
        auto operation = syntax->code[pointer];

//  std::cout << std::string(*operation) << " ";
        ret = operation->Run(this);
        if (ret == ReturnCode::Continue) {
            continue;
        }
        if (ret == ReturnCode::Return) {
            return result;
        }
        ++pointer;
    }
    return result;
}
