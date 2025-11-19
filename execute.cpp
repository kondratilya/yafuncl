#include <iostream>
#include <string>
#include <stdexcept>
#include "execute.h"

Value *Context::SearchVariable(VariableId id) {
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

Value *Context::CreateVariable(VariableId id/*, std::set<Modifyers> modifyers*/, std::string value) {
    Value* val = new Value(static_cast<DefaultValueType>(value));
    val->RemoveModifyers({Modifyers::Defined})->SetModifyers({Modifyers::Lvalue});
    lookup[id] = val;
    return val;
}

Value *Context::GetVariable(VariableId id, std::string value) {
    Value* val = this->SearchVariable(id);
    if (val) return val;
    return CreateVariable(id, value);
}

Context::Context(Context *parent, Address position, Value* arg): Context(*(parent->syntax), position, arg) {
     this->parent = parent;
}

Context::Context(SyntaxAnalys &syntax, Address position, Value* arg) {
    this->parent = NULL;
    this->syntax = &syntax;
    this->pointer = position;
    this->result = new Value("ZERO");
    CreateVariable(0)->Define(arg);
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

Value *Context::Run() {
    while (pointer < syntax->code.size()) {
        auto operation = syntax->code[pointer];

//        std::cout << *operation << " ";

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
                    Push(GetVariable(static_cast<VariableId>(operation->operation), this->syntax->getName(operation->operation)));
                } else {
                    Push(GetVariable(static_cast<VariableId>(operation->operation)));
                }
            break;
            case OperationTypes::Tuple: {
                Tuple *t = new Tuple;
                for (int i=0;i<operation->operation;++i) {
                    t->push_front(Pop());
                }
                Value*v = (new Value(t))->SetModifyers({Modifyers::Lvalue});
                //v->SetLValue(true);
                Push(v);
            } break;
        }
        ++pointer;
    }
    return new Value();
}
