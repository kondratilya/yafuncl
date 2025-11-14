#pragma once
#include <iostream>
#include <istream>
#include <vector>
#include <stack>
#include <string>
#include <list>
#include <stdexcept>
#include "syntax.h"

class Value;

typedef int NameId;
typedef std::string DefaultValueType;
typedef size_t Address;
typedef std::list<Value*> Tuple;

enum class ValueTypes {
    Default,
    Address,
    Tuple,
    Undefined
};

class Value {
    void *value;
    bool defined;
    bool lvalue=false;
    bool plain_tuple=false;
    ValueTypes type;
    public:
    bool GetLValue() { return lvalue; }
    void SetLValue(bool lvalue) { this->lvalue=lvalue; }

    Value(DefaultValueType value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Address value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Tuple *value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Value *value) {
        this->value = value->value;
        this->defined = value->defined;
        this->type = value->type;
        this->lvalue = value->lvalue;
    }
    Value() {
        this->defined=false;
        type = ValueTypes::Undefined;
    }
    void Define(const DefaultValueType value) {
        this->value = new DefaultValueType(value);
        type = ValueTypes::Default;
        defined = true;
        plain_tuple = false;
    }
    void Define(const Address value) {
        this->value = new Address(value);
        type = ValueTypes::Address;
        defined = true;
        plain_tuple = false;
    }
    void Define(Tuple *value, bool plain=true) {
        this->value = value;
        type = ValueTypes::Tuple;
        defined = true;
        plain_tuple = plain;
    }
    void Define(const Value *value) {
        this->value = value->value;
        this->defined = value->defined;
        this->type = value->type;
        this->lvalue = value->lvalue;
        plain_tuple = false;
    }
    DefaultValueType GetValue() {
        return *(DefaultValueType*)value;
    }
    Address GetAddress() {
        if (type!=ValueTypes::Address)
            throw std::runtime_error("Exec: Call to non-address");
        return *(Address*)value;
    }
    friend std::ostream &operator<<(std::ostream &os, Value *v);
};
std::ostream &operator<<(std::ostream &os, Value *v) { 
    if (v->GetLValue())os << "l";
    if (v->plain_tuple)os << "t";
    switch (v->type) {
        case ValueTypes::Default: return os << v->GetValue();
        case ValueTypes::Address: return os << "<" << v->GetAddress() << ">";
        case ValueTypes::Tuple: {
            os << "(";
            for (auto el: *(Tuple*)(v->value)) {
                os << el << ", ";    
            }
            os << ")";
        } return os;
        case ValueTypes::Undefined: return os << "?";
        default: return os << "???";
    }
}

class Context {
    Code *code;
    Context *parent = NULL;
    SyntaxAnalys *syntax;
    std::map<NameId, Value*> lookup;
    std::stack<Value*> stack;
    Address pointer = 0;
    Value *result;

    Value *SearchVariable(NameId id) {
        if (auto search = lookup.find(id); search != lookup.end()) {
            return search->second;
        } else {
            if (this->parent) {
                return this->parent->SearchVariable(id);
            } else {
                return NULL;
            }
        }
    }
    Context(Context *parent, Address position=0, Value* arg=NULL) {
        this->parent = parent;
        this->code = parent->code;
        this->syntax = parent->syntax;
        this->result = new Value("ZERO");
        this->pointer = position;
        Push(arg);
    }
    Value *GetVariable(NameId id, std::string value="") {
        Value* val = this->SearchVariable(id);
        if (val) return val;
        val = new Value(static_cast<DefaultValueType>(value), false);
        val->SetLValue(true);
        lookup[id] = val;
        return val;
    }
    Context *Parent() {
        return parent;
    }
    Value* Pop() {
        if (!stack.size()) 
            return new Value();
        Value *v = stack.top();
        stack.pop();
        return v;
    }
    void Push(Value *v) {
        if (!v) return;
        stack.push(v);
    }
    Value* Top() {
        if (!stack.size()) 
            return new Value();
        return stack.top();
    }

    public:
    Context(Code *code, SyntaxAnalys *syntax=NULL, Address position=0) {
        this->code = code;
        this->pointer = position;
        this->parent = NULL;
        this->result = new Value("ZERO");
        this->syntax = syntax;
    }
    Value *Run() {
        while (pointer < code->size()) {
            auto operation = (*code)[pointer];

            switch (operation->type) {
                case OperationTypes::Operation:
                    switch (static_cast<Operations>(operation->operation)) {
                        case Operations::Jump:
                            pointer = Pop()->GetAddress();
                        continue;
                        case Operations::Call:
                            Push((new Context(this, Pop()->GetAddress()))->Run());
                        break;
                        continue;
                        case Operations::CallArg: {
                            Address address = Pop()->GetAddress();
                            Value *value = Pop();
                            Push((new Context(this, address, value))->Run());
                        } break;
                        case Operations::Return:
                            return result;
                        case Operations::Pop:
                            result = Pop();
                        break;
                        case Operations::Add: 
                            Push(new Value(Pop()->GetValue()+Pop()->GetValue()));
                        break;
                        case Operations::Equate: {
                            Value *v = Pop();
                            // Value *v1 = Top();
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
                    if (syntax) {
                        Push(GetVariable(static_cast<NameId>(operation->operation), this->syntax->getName(operation->operation)));
                    } else {
                        Push(GetVariable(static_cast<NameId>(operation->operation)));
                    }
                break;
                case OperationTypes::Tuple: {
                    Tuple *t = new Tuple;
                    for (int i=0;i<operation->operation;++i) {
                        t->push_front(new Value(Pop()));
                    }
                    Value*v = new Value(t);
                    v->SetLValue(true);
                    Push(v);
                } break;
            }
            ++pointer;
        }
        return new Value();
    }
};

class Execute {
    SyntaxAnalys *syntax;

    public:
    Execute(SyntaxAnalys *syntax) {
        this->syntax = syntax;
        syntax->Analyse();
        for (auto operation : *syntax->code) {
            std::cout << *operation << " ";
        }
        std::cout << std::endl;
    }
    Value *Run() {
        return (new Context(syntax->code, syntax))->Run();
    }
    
};