#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <stdexcept>

#include "values.h"

class SyntaxAnalys;
typedef size_t VariableId;

#include "instructions.h"
#include "syntax.h"


class Context {
    SyntaxAnalys *syntax;
    Context *parent = NULL;
    std::map<VariableId, Values::Value*> lookup;
    std::stack<Values::Value*> stack;
    size_t pointer = 0;
    Values::Value *result;

    public:

    Context *Parent() { return parent; }

    Values::Value *SearchVariable(VariableId id);
    Values::Value *CreateVariable(VariableId id);
    Values::Value *GetVariable(VariableId id);
    Context *WithArgument(Values::Value* arg=NULL);

    Context(Context *parent, size_t position=0);
    
    Values::Value* Pop();
    Values::Value* Top();
    void Push(Values::Value *v);

    Context* Jump(size_t position);
    Context* Jump(const Values::AddressType &address);

    Context* Result(Values::Value *result=NULL);
    
    Context(SyntaxAnalys &syntax, size_t position=0);

    Values::Value *Run();
};
