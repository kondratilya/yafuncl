#pragma once
#include <vector>
#include <stack>
#include <string>
#include "lexic.h"
#include "symbols.h"
#include "rules.h"
#include "operations.h"

typedef size_t VariableId;

#define VERBOSE true

class SyntaxAnalys {
    Rules rules;
    LexicAnalys *lexic;
    bool done = false;

    std::vector<Symbol*> stack;
    std::vector<std::string> names_lookup;
    std::stack<Token*> names_stack;
    std::stack<size_t> function_definitions;
    std::stack<size_t> tuples_stack;

    void RunAction(size_t rule_id);
    bool TestRules(Symbol *S);

    public:
    Code code;

    std::string getName(VariableId id) { return names_lookup[id]; }

    SyntaxAnalys(LexicAnalys &lexic) { this->lexic = &lexic; };

    bool Analyse();
    
};
