#include <iostream>
#include <string>
#include <stdexcept>
#include "context.h"

VariableInfo Context::SearchVariable(VariableId id, AccessType access_type) {
    if (access_type != AccessType::Outer) {
        auto search = lookup.find(id);
        if (search != lookup.end()) {
            return search->second;
        } 
    }
    if (access_type == AccessType::Inner) 
        return VariableInfo{NULL};
    if (this->parent) {
        // return this->parent->SearchVariable(id, AccessType::Default);
        return this->parent->SearchVariable(id, this->parent->access_type_);
    }
    return VariableInfo{NULL};
}

Values::Value *Context::CreateVariable(VariableId id, bool is_mutable) {
    Values::Value* val = (new Values::Value())->Reference(id, this);
    lookup[id] = {val, is_mutable};
    return val;
}

Values::Value *Context::GetVariable(VariableId id, AccessType access_type, MutabilityType mutability_type) {
    bool is_mutable;
    if (mutability_type==MutabilityType::Mutable) is_mutable = true;
    else if (mutability_type==MutabilityType::Immutable) is_mutable = false;
    else is_mutable = vars_default_mutable_;
    if (access_type == AccessType::Default)
        access_type = access_type_;
    if (!parent) 
        access_type = AccessType::Default;
    VariableInfo vi = this->SearchVariable(id, access_type);
    if (vi.value) {
        if(mutability_type!=MutabilityType::Default && (vi.is_mutable != is_mutable))
            throw std::runtime_error("Exec: Cannot override mutability of  \""s + GetVariableName(id) + "\"");
        return vi.value;
    }
    if (access_type == AccessType::Outer) 
        throw std::runtime_error("Exec: Outer variable \""s + GetVariableName(id) + "\" does not exist"s);
    return CreateVariable(id, is_mutable);
}

bool Context::IsVariableMutable(VariableId id) {
    return lookup[id].is_mutable;
}

std::string Context::GetVariableName(VariableId id) {
    return syntax->GetVariableName(id);
}

Context::Context(Context *parent, size_t position): Context(*(parent->syntax), position, parent->result_stream_, parent->output_stream_) {
    this->parent = parent;
}

Context::Context(Context *parent, size_t position, Values::Value* arguments): Context(*(parent->syntax), position, parent->result_stream_, parent->output_stream_) {
    this->parent = parent;
    CreateVariable(syntax->arguments_id, false)->SetTo(arguments);
}

Context::Context(SyntaxAnalys &syntax, size_t position, std::ostream *result_stream, std::ostream *output_stream) {
    this->parent = NULL;
    this->syntax = &syntax;
    this->pointer = position;
    result_stream_ = result_stream;
    output_stream_ = output_stream;
    Result();
}

Values::Value *Context::Pop(bool is_to_abyss) {
    if (!stack.size()) {
        // std::cout << "POP EMPTY!";
        return new Values::Value();
    }
    Values::Value *v = stack.top();
    stack.pop();
    if (is_to_abyss) {
        abyss_.push(v);
    }
    return v;
}

Values::Value *Context::Top() {
    if (!stack.size()) {
        // std::cout << "TOP EMPTY!";
        return new Values::Value();
    }
    return stack.top();
}

void Context::Push(Values::Value *v) {
    if (!v) return;
    stack.push(v);
}

Context* Context::Jump(const Values::AddressType &address) {
    pointer = address.position;
    return this;
}

Context* Context::SetInnerAccess() {
    access_type_ = AccessType::Inner;
    return this;
}

Context* Context::SetMutableVars() {
    vars_default_mutable_ = true;
    return this;
}

Context* Context::Result(Values::Value *result) {
    if (result) {
        this->result = result;
    } else {
        this->result = new Values::Value(Values::DefaultValueType(0));    // Неявное определение 0 в языке тут
    }
    return this;
}

Values::Value *Context::Run() {
    ReturnCode ret = ReturnCode::None;
    while (pointer < syntax->code.size()) {
        auto instruction = syntax->code[pointer];

//   std::cout << std::string(*instruction) << " ";
        ret = instruction->Run(this);
        // while (abyss_.size()) {
        //     Values::Value* v = abyss_.top();
        //     Values::Value::Delete(v);
        //     std::cout << v;
        //     abyss_.pop();
        // }

        if (ret == ReturnCode::Continue) {
            continue;
        }
        if (ret == ReturnCode::Return) {
            Values::Value *new_result = new Values::Value(result);
            Result(NULL);          
            return new_result;
        }
        ++pointer;
    }
    Result(NULL);
    return new Values::Value(result);
}