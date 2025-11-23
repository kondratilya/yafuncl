#include <iostream>
#include <string>
#include <stdexcept>
#include "context.h"

Values::Value *Context::SearchVariable(VariableId id) {
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

Values::Value *Context::CreateVariable(VariableId id) {
    Values::Value* val = new Values::Value();
    lookup[id] = val;
    return val;
}

Values::Value *Context::GetVariable(VariableId id) {
    Values::Value* val = this->SearchVariable(id);
    if (val) return val;
    return CreateVariable(id);
}

Context::Context(Context *parent, size_t position): Context(*(parent->syntax), position) {
     this->parent = parent;
}

Context::Context(SyntaxAnalys &syntax, size_t position) {
    this->parent = NULL;
    this->syntax = &syntax;
    Jump(position);
    Result();
}

Context *Context::WithArgument(Values::Value* arg) {
    CreateVariable(syntax->arguments_id)->SetTo(arg);
    return this;
}

Values::Value *Context::Pop() {
    if (!stack.size()) 
        return new Values::Value();
    Values::Value *v = stack.top();
    stack.pop();
    return v;
}

Values::Value *Context::Top() {
    if (!stack.size()) 
        return new Values::Value();
    return stack.top();
}

void Context::Push(Values::Value *v) {
    if (!v) return;
    stack.push(v);
}

Context* Context::Jump(size_t position) {
    pointer = position;
    return this;
}

Context* Context::Jump(const Values::AddressType &address) {
    pointer = address.position;
    return this;
}

Context* Context::Result(Values::Value *result) {
    if (result) {
        this->result = result;
    } else {
        this->result = new Values::Value(Values::DefaultValueType(0));    // Неявное определение 0 в языку тут
    }
    return this;
}

Values::Value *Context::Run() {
    ReturnCode ret = ReturnCode::None;
    while (pointer < syntax->code.size()) {
        auto instruction = syntax->code[pointer];

// std::cout << std::string(*instruction) << " ";
        ret = instruction->Run(this);
        if (ret == ReturnCode::Continue) {
            continue;
        }
        if (ret == ReturnCode::Return) {
            Values::Value *new_result = new Values::Value(result);
            if (new_result->type == Values::ValueTypes::Address) {
                new_result->GetAddress().SetContext(this);
            }
            Result(NULL);
            return new_result;
        }
        ++pointer;
    }
    Result(NULL);
    return new Values::Value(result);
}