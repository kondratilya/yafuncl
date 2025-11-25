#pragma once
#include <vector>
#include <stack>
#include <string>
#include "lexic.h"
#include "symbols.h"
#include "modifyers.h"
#include "rules.h"
#include "variables_table.h"
#include "instructions.h"

class SyntaxAnalys {
    Rules rules;
    LexicAnalys *lexic;

    std::vector<Symbol*> stack;
    std::vector<size_t> heads;

    VariablesLookup names_lookup;
    std::stack<std::string> names_stack;
    std::stack<size_t> function_definitions;
    size_t tuples_counter_ = 0;
    Modifyers modifyers_;

    void RunAction(size_t rule_id);
    bool FindBeginning(Symbol *S, size_t &rule);
    bool FindReduction(size_t &rule, size_t &head);
    bool TestRules(Symbol *S);

    bool verbose_;

    public:
    Code code;
    size_t arguments_id = 0;

    SyntaxAnalys(LexicAnalys &lexic, bool verbose=false) { 
        this->lexic = &lexic; 
        verbose_ = verbose;
        arguments_id = names_lookup.insert("arguments");
    };
    std::string GetVariableName(VariableId id) {
        return names_lookup.get(id);
    }
    std::string StackToStr(Symbol *S=NULL);
    bool Analyse();
    
};
