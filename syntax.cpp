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
        case Actions::Name:
            code.push_back(new VariableInstruction(
                    this->names_lookup.insert(names_stack.top()), 
                    modifyers_, 
                    &this->names_lookup));
            names_stack.pop();
            modifyers_.clear();
        break;
        case Actions::ModifyerInner: 
            modifyers_.insert(Modifyer::Inner);
        break;
        case Actions::ModifyerOuter: 
            modifyers_.insert(Modifyer::Outer);
        break;
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
            code.push_back(new Instruction());
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
        case Actions::ThenEnd:
            function_definitions.pop();
        break;
        case Actions::Else: {
            size_t true_address = function_definitions.top();
            function_definitions.pop();
            delete code[true_address];
            code[true_address] = new AddressInstruction(code.size());
            delete code[true_address+2];
            code[true_address+2] = new OperatorInstruction(Operators::Pass);
        } break;
        case Actions::While:
            function_definitions.push(code.size());     // before condition: after loop jump here
        break;
        case Actions::WhileDo:
            function_definitions.push(code.size());
            code.push_back(new Instruction());          // end: for false - jump to end
            code.push_back(new OperatorInstruction(Operators::Jz));
        break;
        case Actions::WhileEnd: {
            size_t end_jump_position = function_definitions.top();
            function_definitions.pop();
            size_t loop_start_position = function_definitions.top();
            function_definitions.pop();
            code.push_back(new AddressInstruction(loop_start_position));
            code.push_back(new OperatorInstruction(Operators::Jump));
            code[end_jump_position] = new AddressInstruction(code.size());
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
        case Actions::CloneTuple:
            code.push_back(new OperatorInstruction(Operators::Clone));
        break;
        case Actions::Index:
            code.push_back(new OperatorInstruction(Operators::Index));
        break;
        case Actions::ToTuple:
            code.push_back(new OperatorInstruction(Operators::ToTuple));
        break;
        case Actions::Length:
            code.push_back(new OperatorInstruction(Operators::Length));
        break;
        case Actions::Print: 
            code.push_back(new OperatorInstruction(Operators::Print));
        break;
        case Actions::PrintLn: 
            code.push_back(new OperatorInstruction(Operators::Print));
            code.push_back(new OperatorInstruction(Operators::PrintLf));
        break;
        case Actions::PrintChar: 
            code.push_back(new OperatorInstruction(Operators::PrintChar));
        break;
        case Actions::BeginFunction: 
            function_definitions.push(code.size());
            code.push_back(new Instruction());
            code.push_back(new OperatorInstruction(Operators::Jump));
            if (modifyers_.count(Modifyer::Outer)) 
                throw std::runtime_error("Syntax: Outer modifyer for function");
            if (modifyers_.count(Modifyer::Inner)) 
                code.push_back(new OperatorInstruction(Operators::InnerAccess));
            modifyers_.clear();
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
            function_definitions.push(code.size());
            code.push_back(new AddressInstruction(code.size()+2));
            code.push_back(new OperatorInstruction(Operators::Jump));

            code.push_back(new OperatorInstruction(Operators::Pop));
            code.push_back(new OperatorInstruction(Operators::Return));

            code.push_back(new AddressInstruction(code.size()+2));
            code.push_back(new OperatorInstruction(Operators::Jump));          
        break;
        case Actions::ReturnEmpty:
            function_definitions.push(code.size());
            code.push_back(new AddressInstruction(code.size()+2));
            code.push_back(new OperatorInstruction(Operators::Jump));

            code.push_back(new OperatorInstruction(Operators::Return));

            code.push_back(new AddressInstruction(code.size()+2));
            code.push_back(new OperatorInstruction(Operators::Jump));
        break;
        case Actions::ReturnIf: {
            size_t skip_to_condition = function_definitions.top();
            function_definitions.pop();
            delete code[skip_to_condition];
            code[skip_to_condition] = new AddressInstruction(skip_to_condition + 6);
            code.push_back(new AddressInstruction(skip_to_condition+2));
            code.push_back(new OperatorInstruction(Operators::Jnz));
            code.push_back(new OperatorInstruction(Operators::Pop));
            delete code[skip_to_condition+4];
            code[skip_to_condition+4] = new AddressInstruction(code.size());
        } break;
        case Actions::ReturnEmptyIf: {
            size_t skip_to_condition = function_definitions.top();
            function_definitions.pop();
            delete code[skip_to_condition];
            code[skip_to_condition] = new AddressInstruction(skip_to_condition + 5);
            code.push_back(new AddressInstruction(skip_to_condition+2));
            code.push_back(new OperatorInstruction(Operators::Jnz));
            delete code[skip_to_condition+3];
            code[skip_to_condition+3] = new AddressInstruction(code.size());
        } break;
        case Actions::ReturnUnconditional: 
            function_definitions.pop();
            delete code.back();
            code.pop_back();
            delete code.back();
            code.pop_back();
        break;
    }
}

bool SyntaxAnalys::FindBeginning(Symbol *S, size_t &rule) {
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
                    rule = r;
                    return true;
                }
            }
        }
    }
    return false;
}

bool SyntaxAnalys::FindReduction(size_t &rule, size_t &head) {
    bool found = true;
    size_t max_depth=2;
    int start_head = heads.size()-(max_depth+1);
    if (start_head < 0) start_head=0;
    
    for (size_t h=start_head; h<heads.size(); ++h) {
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
                rule=r;
                head=h;
                return true;
            }
        }
    }
    return false;    
}

bool SyntaxAnalys::TestRules(Symbol *S) {
    size_t beginning, rule, head;

    if (FindBeginning(S, beginning)) {
        return true;
    }

    if (FindReduction(rule, head)) {
        if (verbose_) std::cout << " [" << std::setw(2) << rule << "] ";

        if (rules[rule].left==NT::ROOT && stack.size() != rules[rule].right.size()) {
            throw std::runtime_error("Syntax: Early end\n"s + StackToStr(S));
        }
        for (size_t i=0; i<rules[rule].right.size(); ++i) {
            delete stack.back();
            stack.pop_back();
        }
        stack.push_back(new Symbol(rules[rule].left));
        heads.resize(head+1);

        RunAction(rule);

        return(false);
    }

    heads.push_back(stack.size());
    if (*S==Symbol(Tokens::END)) {
        throw std::runtime_error("Syntax: Error\n"s + StackToStr(S));
    }
    if (verbose_) std::cout << std::setw(2) << " [  ] ";
    return(true);

}

std::string SyntaxAnalys::StackToStr(Symbol *S) {
    std::ostringstream os;
    for (auto i = 0; i<stack.size(); ++i) {
        for (auto head : heads) {
            if(head==i)os << ".";
        }
        os << *stack[i] << " ";
    }
    if (S) os << "  <-- " << *S;
    return os.str();
}

bool SyntaxAnalys::Analyse() {
    bool to_read=true;
    Token *R;
    Symbol *S;

    if (verbose_) std::cout << std::endl;

    while(true){
        if(to_read) {
            R = this->lexic->Get();
            if (*R=="return") {
                Token *R_next = this->lexic->ShowNext();
                if (*R_next==";" || *R_next=="if") {
                    delete R;
                    this->lexic->UnGet("$$$returnempty$$$");
                    continue;
                }
            }
            S = new Symbol(R);
            if (S->IsName()) {
                names_stack.push(R->Value());
            }
        }

        to_read=TestRules(S);
        if(to_read)
            stack.push_back(S);

        if (verbose_) {
            std::cout << StackToStr(S) << std::endl;
        }

        if(stack.size()==1 && *stack[0] == Symbol(NT::ROOT)) {       
            return true;
        } 
    }
    return false;
}
    

