#include "syntax.h"

#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <stdexcept>
#include "symbols.h"
#include "rules.h"
#include "instructions.h"

#include <iomanip>

using namespace Symbols;

void SyntaxAnalys::RunAction(size_t rule_id) {
    switch (rules[rule_id].action) {
        case Actions::Name: {
            std::string name = names_stack.top();
            names_stack.pop();
            code.push_back(new VariableInstruction(this->names_lookup.insert(name), &this->names_lookup));
        } break;
        case Actions::TupleEmpty: 
            code.push_back(new TupleInstruction(0));
        break;
        case Actions::TupleOne:
            code.push_back(new TupleInstruction(1));
        break;
        case Actions::Tuple:
            tuples_counter_++;
        break;
        case Actions::TupleEnd: 
            code.push_back(new TupleInstruction(tuples_counter_ + 1));
            tuples_counter_ = 0;
        break;
        case Actions::Pop: 
            code.push_back(new OperatorInstruction(Operators::Pop));
        break;            
        case Actions::If:
            function_definitions.push(code.size());
            code.push_back(new Instruction());                         //
            code.push_back(new OperatorInstruction(Operators::Jz));
        break;

        case Actions::Then: {
            size_t false_address = function_definitions.top();
            function_definitions.pop();
            delete code[false_address];
            function_definitions.push(code.size());
            code.push_back(new AddressInstruction(code.size()+3));     // reserved for "Else"
            code.push_back(new OperatorInstruction(Operators::Jump));
            code[false_address] = new AddressInstruction(code.size());
            code.push_back(new EmptyValueInstruction()); 
        } break;
        case Actions::Else: {
            size_t true_address = function_definitions.top();
            function_definitions.pop();
            delete code[true_address];
            code[true_address] = new AddressInstruction(code.size());
            delete code[true_address+2];
            code[true_address+2] = new OperatorInstruction(Operators::Pass);
        } break;
        case Actions::Plus:
            code.push_back(new OperatorInstruction(Operators::Add));
        break;
        case Actions::Minus:
            code.push_back(new OperatorInstruction(Operators::Substract));
        break;
        case Actions::Multiply: 
            code.push_back(new OperatorInstruction(Operators::Multiply));
        break;
        case Actions::Divide: 
            code.push_back(new OperatorInstruction(Operators::Divide));
        break;
        case Actions::Mod: 
            code.push_back(new OperatorInstruction(Operators::Mod));
        break;
        case Actions::Negative: 
            code.push_back(new OperatorInstruction(Operators::Negative));
        break;
        case Actions::Positive: 
            code.push_back(new OperatorInstruction(Operators::Positive));
        break;
        case Actions::Dec: 
            code.push_back(new OperatorInstruction(Operators::Dec));
        break;
        case Actions::Inc: 
            code.push_back(new OperatorInstruction(Operators::Inc));
        break;
        case Actions::Not: 
            code.push_back(new OperatorInstruction(Operators::Not));
        break;
        case Actions::Or: 
            code.push_back(new OperatorInstruction(Operators::Or));
        break;
        case Actions::And: 
            code.push_back(new OperatorInstruction(Operators::And));
        break;
        case Actions::IsEqual: 
            code.push_back(new OperatorInstruction(Operators::IsEqual));
        break;
        case Actions::IsNotEqual: 
            code.push_back(new OperatorInstruction(Operators::IsNotEqual));
        break;
        case Actions::IsLess: 
            code.push_back(new OperatorInstruction(Operators::IsLess));
        break;
        case Actions::IsMore: 
            code.push_back(new OperatorInstruction(Operators::IsMore));
        break;
        case Actions::IsLessOrEqual: 
            code.push_back(new OperatorInstruction(Operators::IsLessOrEqual));
        break;
        case Actions::IsMoreOrEqual: 
            code.push_back(new OperatorInstruction(Operators::IsMoreOrEqual));
        break;
        case Actions::Equate:
            code.push_back(new OperatorInstruction(Operators::Equate));
        break;
        case Actions::Unpack:
            code.push_back(new OperatorInstruction(Operators::Unpack));
        break;
        case Actions::Print: 
            code.push_back(new OperatorInstruction(Operators::Print));
        break;
        case Actions::PrintLn: 
            code.push_back(new OperatorInstruction(Operators::Print));
            code.push_back(new OperatorInstruction(Operators::PrintLf));
        break;
        case Actions::BeginFunction: 
            function_definitions.push(code.size());
            code.push_back(new Instruction());
            code.push_back(new OperatorInstruction(Operators::Jump));
        break;
        case Actions::EndFunction: { 
            size_t function_start = function_definitions.top();
            function_definitions.pop();
            code.push_back(new OperatorInstruction(Operators::Return));
            delete code[function_start];
            code[function_start] = new AddressInstruction(code.size());
            code.push_back(new AddressInstruction(function_start+2));
        } break;
        case Actions::FunctionCall: 
            code.push_back(new OperatorInstruction(Operators::Call));
        break;
        case Actions::FunctionCallArg:
            code.push_back(new OperatorInstruction(Operators::CallArg));
        break;
        case Actions::Return:
            code.push_back(new OperatorInstruction(Operators::Pop));
            code.push_back(new OperatorInstruction(Operators::Return));
        break;
        case Actions::ReturnEmpty:
            code.push_back(new OperatorInstruction(Operators::Return));
            // lexic->UnGet(";");
            stack.push_back(new Symbol(";"));
        break;
    }
}

bool SyntaxAnalys::TestRules(Symbol *S) {
    bool beginning;

    int search_from = stack.size()-rules.MaxRuleSize();
    if (search_from < 0)search_from=0;

    for (size_t i=search_from; i<stack.size(); ++i){
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
                    if (verbose_) std::cout << "B[" << std::setw(2) << r << "] ";
                    return true;
                }
            }
        }
    }

    for (size_t h=0; h<heads.size(); ++h) {
        for (size_t r=0; r<rules.size(); ++r){
            bool found = true;
            if (rules[r].right.size() != stack.size()-heads[h])
                continue;
            for (size_t i=0; i<rules[r].right.size() && i< stack.size()+heads[h]; ++i) {
                if (rules[r].right[i] != *stack[i+heads[h]]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                if (verbose_) std::cout << " [" << std::setw(2) << r << "] ";
                if (rules[r].left==NT::ROOT && stack.size() != rules[r].right.size()) {
                    throw std::runtime_error("Syntax: Early end");
                }
                for (size_t i=0; i<rules[r].right.size(); ++i) {
                    delete stack.back();
                    stack.pop_back();
                }
                stack.push_back(new Symbol(rules[r].left));
                heads.resize(h+1);

                RunAction(r);
                return(false);

            }
        }
    }

    heads.push_back(stack.size());
    if (*S==Symbol(Tokens::END)) {
        throw std::runtime_error("Syntax: Error");
    }
    if (verbose_) std::cout << std::setw(2) << " [  ] ";
    return(true);

}

bool SyntaxAnalys::Analyse() {
    bool to_read=true;
    Token *R;
    Symbol *S;

    if (verbose_) std::cout << std::endl;

    while(true){
        if(to_read) {
            R = this->lexic->Get();
            S = new Symbol(R);
            if (S->IsName()) {
                names_stack.push(R->Value());
            }
        }

        to_read=TestRules(S);
        if(to_read)
            stack.push_back(S);

        if (verbose_) {
            for (auto i = 0; i<stack.size(); ++i) {
                for (auto head : heads) {
                    if(head==i)std::cout << ".";
                }
                std::cout << *stack[i] << " ";
            }
            std::cout << "  <-- " << *S << std::endl;
        }

        if(stack.size()==1 && *stack[0] == Symbol(NT::ROOT)) {       
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
    

