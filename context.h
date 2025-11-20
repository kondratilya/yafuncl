#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <stdexcept>

class Value;
typedef size_t Address;
class SyntaxAnalys;
typedef size_t VariableId;

#include "instructions.h"
#include "syntax.h"
#include "values.h"

class Context {
    SyntaxAnalys *syntax;
    Context *parent = NULL;
    std::map<VariableId, Value*> lookup;
    std::stack<Value*> stack;
    Address pointer = 0;
    Value *result;

    public:

    Context *Parent() { return parent; }

    Value *SearchVariable(VariableId id);
    Value *CreateVariable(VariableId id);
    Value *GetVariable(VariableId id);

    Context(Context *parent, Address position=0, Value* arg=NULL);
    
    Value* Pop();
    Value* Top();
    void Push(Value *v);

    void Jump(Address a);

    void Result(Value *result);
    
    Context(SyntaxAnalys &syntax, Address position=0, Value* arg=NULL);

    Value *Run();
};
