#include "syntax.h"

#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "symbols.h"
#include "rules.h"
#include "operations.h"

#define VERBOSE true

using namespace Symbols;

void SyntaxAnalys::RunAction(size_t rule_id) {
    switch (rules[rule_id].action) {
        case Actions::Name: {
            std::string name = names_stack.top()->Value();
            auto it = std::find(names_lookup.begin(), names_lookup.end(), name);
            int name_index;
            if (it == names_lookup.end()) {
                names_lookup.push_back(name);
                name_index = names_lookup.size()-1;
            } else {
                name_index = std::distance(names_lookup.begin(), it);
            }
            code.push_back(new Operation(OperationTypes::NameLookup, name_index));
            names_stack.pop();
        } break;
        case Actions::TupleEmpty: 
            std::cout << "[[Tuple0]]";
            code.push_back(new Operation(OperationTypes::Tuple, 0));
        break;
        case Actions::TupleOne:
            std::cout << "[[Tuple0]]";
            code.push_back(new Operation(OperationTypes::Tuple, 1));
        break;
        case Actions::TupleStart: {
            tuples_stack.push(2);
        } break;
        case Actions::Tuple: {
            size_t tuple_size = tuples_stack.top()+1;
            tuples_stack.pop();
            tuples_stack.push(tuple_size);
            // Operation *tuple = code.back();
            // if (tuple->type!=OperationTypes::Tuple) {
            //     std::cout << "[["<<rule_id<<"Tuple1]]";
            //     code.push_back(new Operation(OperationTypes::Tuple, 1));
            //     break;
            // }
            // tuple->operation++;
            // std::cout << "[["<<rule_id<<"Tuple" <<tuple->operation << "]]";
        } break;
        case Actions::TupleEnd: {
            code.push_back(new Operation(OperationTypes::Tuple, tuples_stack.top()));
            tuples_stack.pop();
        } break;
        case Actions::Pop: 
            code.push_back(new Operation(Operations::Pop));
        break;            
        case Actions::Plus: 
            code.push_back(new Operation(Operations::Add));
        break;
        case Actions::Equate: 
            code.push_back(new Operation(Operations::Equate));
        break;
        case Actions::Print: 
            code.push_back(new Operation(Operations::Print));
        break;
        case Actions::BeginFunction: 
            function_definitions.push(code.size());
            code.push_back(new Operation());
            code.push_back(new Operation(Operations::Jump));
        break;
        case Actions::EndFunction: { 
            size_t function_start = function_definitions.top();
            function_definitions.pop();
            code.push_back(new Operation(Operations::Return));
            code[function_start]->type = OperationTypes::Address;
            code[function_start]->operation = code.size();
            code.push_back(new Operation(OperationTypes::Address, function_start+2));
        } break;
        case Actions::FunctionCall: 
            code.push_back(new Operation(Operations::Call));
        break;
        case Actions::FunctionCallArg:
            code.push_back(new Operation(Operations::CallArg));
        break;
    }
}

bool SyntaxAnalys::TestRules(Symbol *S) {
    bool beginning;
    for (size_t i=0; i<stack.size(); ++i){
        for (size_t r=0; r<rules.size(); ++r){
            beginning=true;
            size_t j;

            for (j=i; j<stack.size(); ++j){
                if (rules[r].right.size()<=j-i || rules[r].right[j-i] != *stack[j]){
                    beginning=false;
                    break;
                }
            }
            if (beginning) {
                if (rules[r].right.size()>j-i && rules[r].right[j-i] == *S) {
                    return true;
                }
            }
        }
    }
    for (size_t i=0; i<stack.size(); ++i) {
        for (size_t r=0; r<rules.size(); ++r) {
            beginning=true;
            size_t j;

            for (j=i; j<stack.size(); ++j) {
                if (rules[r].right.size()<=j-i || rules[r].right[j-i] != *stack[j]) {
                    beginning=false;
                    break;
                }
            }
            if (beginning && rules[r].right.size() == stack.size()-i) {
                if (rules[r].left==Symbol(NT::ROOT) && i!=0) {
                    throw std::runtime_error("Syntax: Early end");
                }
                stack.resize(i);
                stack.push_back(new Symbol(rules[r].left));

                RunAction(r);

                return(false);
            }
        }

    }
    if (*S==Symbol(Tokens::END)) {
        throw std::runtime_error("Syntax: Error");
    }
    return(true);

}

bool SyntaxAnalys::Analyse() {
    if (done) {
        return true;
    }
    bool to_read=true;
    Token *R;
    Symbol *S;

    while(true){
        if(to_read) {
            R = this->lexic->Get();
            S = new Symbol(R);
            if (S->IsName()) {
                names_stack.push(R);
            }
        }

        to_read=TestRules(S);
        if(to_read)
            stack.push_back(S);

#ifdef VERBOSE
        for (auto i : stack) {
            std::cout << *i << " ";
        }
        std::cout << "  <-- " << *S << std::endl;
#endif

        if(stack.size()==1 && *stack[0] == Symbol(NT::ROOT)) {
            done = true;            
            return true;

            /*if (Token(END)==R) {
                std::cout << " <OK>";
                return true;
            } else {
                throw std::runtime_error("Syntax: Early end");
                return false;
            }*/
        } 
    }
    return false;
}
    

