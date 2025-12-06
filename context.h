#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <stdexcept>
#include "access_types.h"
#include "values.h"

class SyntaxAnalys;
typedef size_t VariableId;

#include "instructions.h"
#include "syntax.h"

struct VariableInfo {
    Values::Value *value=NULL;
    bool is_mutable = false;
};

class Context {
    SyntaxAnalys *syntax;
    Context *parent = NULL;
    std::map<VariableId, VariableInfo> lookup;
    std::stack<Values::Value*> stack;
    size_t pointer = 0;
    Values::Value *result;
    AccessType access_type_ = AccessType::Default;
    bool vars_default_mutable_ = false;
    std::ostream *output_stream_;
    std::ostream *result_stream_;

    std::stack<Values::Value*> abyss_;
    public:

    Context(SyntaxAnalys &syntaxm, size_t position=0, std::ostream *result_stream=NULL, std::ostream *output_stream=NULL);

    ~Context() {
        // Контекст должен удалить значения из своей таблицы, но пока контексты не удаляю
    }

    VariableInfo SearchVariable(VariableId id, AccessType access_type=AccessType::Default);
    Values::Value *CreateVariable(VariableId id, bool is_mutable=false);
    Values::Value *GetVariable(VariableId id, AccessType access_type=AccessType::Default, MutabilityType mutability_type=MutabilityType::Default);
    bool IsVariableMutable(VariableId id);
    std::string GetVariableName(VariableId id);

    Context(Context *parent, size_t position=0);
    Context(Context *parent, size_t position, Values::Value* arguments);
    
    Values::Value* Pop(bool is_to_abyss=true);
    void PopDelete();
    Values::Value* Top();
    void Push(Values::Value *v);

    Context* Jump(const Values::AddressType &address);

    Context* Result(Values::Value *result=NULL);

    Context* SetInnerAccess();
    Context* SetMutableVars();

    void Output(std::string output) {
        if (result_stream_) {
            *result_stream_ << output;
        }
    }

    Values::Value *Run();
};
