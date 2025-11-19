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

    std::vector<Symbol*> stack;
    std::vector<size_t> heads;

    std::vector<std::string> names_lookup;
    std::stack<std::string> names_stack;
    std::stack<size_t> function_definitions;
    std::stack<size_t> tuples_stack;

    void RunAction(size_t rule_id);
    bool TestRules(Symbol *S);

    public:
    Code code;

    std::string getName(VariableId id) { return names_lookup[id]; }

    SyntaxAnalys(LexicAnalys &lexic) { 
        this->lexic = &lexic; 
        names_lookup.push_back("arguments");
    };

    bool Analyse();
    
};
