#include <iostream>
#include <map>
#include "instructions.h"
#include "symbols.h"
#include "values.h"

using namespace Symbols;
std::map<NonTerminals, std::string> str_non_terminals = {
    {NonTerminals::ROOT, "ROOT"}, {NonTerminals::E, "E"}, 
    {NonTerminals::T0, "T0"}, {NonTerminals::T1, "T1"}, {NonTerminals::T2, "T2"}, 
    {NonTerminals::T3, "T3"}, {NonTerminals::T4, "T4"}, {NonTerminals::T5, "T5"},
    {NonTerminals::T6, "T6"}, {NonTerminals::T7, "T7"}, {NonTerminals::T8, "T8"},
    {NonTerminals::T9, "T9"}, {NonTerminals::T10, "T10"}, {NonTerminals::T11, "T11"},
    {NonTerminals::T12, "T12"}, {NonTerminals::T13, "T13"},
    {NonTerminals::LVALUE, "LVALUE"}, 
    {NonTerminals::NAME, "NAME"}, 
    {NonTerminals::Program, "Program"},
    {NonTerminals::Expressions, "Expressions"},
    {NonTerminals::Expression, "Expression"},
    {NonTerminals::BlockBegin, "BlockBegin"}, {NonTerminals::Block, "Block"},  
    {NonTerminals::FunctionCall, "FunctionCall"}, 
    {NonTerminals::Tuple, "Tuple"}, {NonTerminals::TupleDone, "TupleDone"},
    {NonTerminals::Modifyers, "Modifyers"}, 
    {NonTerminals::Modifyer, "Modifyer"}, 
};
std::ostream &operator<<(std::ostream &os, Symbol &s) { 
    if (s.IsTerminal()) {
        if (s.IsName())
            return os << "[" << s.token->Value() << "]";
        else
            return os << s.name;
    } else  {
        return os << "<" << str_non_terminals[s.non_terminal] << ">";
    }
}


std::ostream &operator<<(std::ostream &os, Value *v) {
    switch (v->type) {
        case ValueTypes::Default: return os << v->GetValue(NULL);
        case ValueTypes::Address: return os << "<" << v->GetAddress() << ">";
        case ValueTypes::Tuple: {
            os << "(";
            for (auto el: *(Tuple*)(v->value)) {
                os << el << ", ";    
            }
            os << ")";
        } return os;
        case ValueTypes::Undefined: return os << "?";
        default: return os << "???";
    }
}
