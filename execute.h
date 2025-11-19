#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <stdexcept>
#include "syntax.h"
#include "values.h"

class Context {
    SyntaxAnalys *syntax;
    Context *parent = NULL;
    std::map<VariableId, Value*> lookup;
    std::stack<Value*> stack;
    Address pointer = 0;
    Value *result;

    Context *Parent() { return parent; }

    Value *SearchVariable(VariableId id);
    Value *CreateVariable(VariableId id, std::string value="");
    Value *GetVariable(VariableId id, std::string value="");

    Context(Context *parent, Address position=0, Value* arg=NULL);
    
    Value* Pop();
    Value* Top();
    void Push(Value *v);

    public:
    Context(SyntaxAnalys &syntax, Address position=0, Value* arg=NULL);

    Value *Run();
};

class Execute {
    SyntaxAnalys *syntax;

    public:
    Execute(SyntaxAnalys &syntax) {
        this->syntax = &syntax;
        this->syntax->Analyse();
        for (auto operation : this->syntax->code) {
            std::cout << *operation << " ";
        }
        std::cout << std::endl;
    }
    Value *Run() {
        return (new Context(*syntax))->Run();
    }
};