#pragma once
#include <iostream>
#include <istream>
#include <vector>
#include <stack>
#include <string>
#include <stdexcept>
#include "syntax.h"

typedef int NameId;
typedef std::string DefaultValueType;
typedef size_t Address;

enum class ValueTypes {
    Default,
    Address,
    Undefined
};

class Value {
    void *value;
    bool defined;
    ValueTypes type;
    public:
    Value(DefaultValueType value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Address value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value() {
        this->defined=false;
        type = ValueTypes::Undefined;
    }
    void Define(const DefaultValueType value) {
        this->value = new DefaultValueType(value);
        type = ValueTypes::Default;
        defined = true;
    }
    void Define(const Address value) {
        this->value = new Address(value);
        type = ValueTypes::Address;
        defined = true;
    }
    void Define(const Value value) {
        Define(&value);
     }
    void Define(const Value *value) {
        this->value = value->value;
        this->defined = value->defined;
        this->type = value->type;
    }
    DefaultValueType GetValue() {
        return *(DefaultValueType*)value;
    }
    Address GetAddress() {
        return *(Address*)value;
    }
    friend std::ostream &operator<<(std::ostream &os, Value *v);
};
std::ostream &operator<<(std::ostream &os, Value *v) { 
    switch (v->type) {
        case ValueTypes::Default: return os << v->GetValue();
        case ValueTypes::Address: return os << "<" << v->GetAddress() << ">";
        case ValueTypes::Undefined: return os << "?";
        default: return os << "???";
    }
}

class Context {
    Context *parent;
    std::map<NameId, Value*> lookup;
    Value *Search(NameId id) {
        if (auto search = lookup.find(id); search != lookup.end()) {
            return search->second;
        } else {
            if (this->parent) {
                return this->parent->Search(id);
            } else {
                return NULL;
            }
        }
    }
    public:
    Context(Context *parent) {
        this->parent = parent;
        this->result = new Value("ZERO");
    }
    Value *GetValue(NameId id, std::string value="") {
        Value* val = this->Search(id);
        if (val) return val;
        val = new Value(static_cast<DefaultValueType>(value), false);
        lookup[id] = val;
        return val;
    }
    Context *Parent() {
        return parent;
    }
    Value *result;
};

class Execute {
    std::stack<Value*> stack;
    SyntaxAnalys *syntax;
    Value* Pop() {
        Value *v = stack.top();
        stack.pop();
        return v;
    }
    void Push(Value *v) {
        stack.push(v);
    }
    Value* Top() {
        return stack.top();
    }
    public:
    Execute(SyntaxAnalys *syntax) {
        this->syntax = syntax;
        syntax->Analyse();
    }
    void Run() {
        Context *context=NULL;
        std::stack<Address> call_stack;

        for (auto operation : syntax->code) {
            std::cout << *operation << " ";
        }
        std::cout << std::endl;

        size_t pointer=0;

        while (pointer < syntax->code.size()) {
            auto operation = syntax->code[pointer];
 // std::cout << *operation << " ";
            switch (operation->type) {
                case OperationTypes::Operation:
                    switch (static_cast<Operations>(operation->operation)) {
                        case Operations::CreateContext:
                            context = new Context(context);
                        break;
                        case Operations::DestroyContext: {
                            Context *parent = context->Parent();
                            Push(context->result);
                            delete context;
                            context = parent;
                        } break;
                        case Operations::Jump:
                            pointer = Pop()->GetAddress();
                        continue;
                        case Operations::Call:
                            call_stack.push(pointer);
                            pointer = Pop()->GetAddress();
                        continue;
                        case Operations::Return:
                            pointer = call_stack.top()+1;
                            call_stack.pop();
                        continue;
                        case Operations::Pop:
                            context->result = Pop();
                        break;
                        case Operations::Add: 
                            Push(new Value(Pop()->GetValue()+Pop()->GetValue()));
                        break;
                        case Operations::Equate: {
                            Value *v = Pop();
                            Top()->Define(v);
                        } break;
                        case Operations::Print: {
                            std::cout << "\"" << Top() << "\" ";
                        }break;                        
                    }
                break;
                case OperationTypes::Address:
                    Push(new Value(static_cast<Address>(operation->operation)));
                break;
                case OperationTypes::NameLookup:
                    Push(context->GetValue(static_cast<NameId>(operation->operation), this->syntax->getName(operation->operation)));
                break;
            }
            ++pointer;
        }
    }
};