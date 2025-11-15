#include <iostream>
#include <map>
#include "operations.h"
#include "symbols.h"
#include "values.h"

std::map<Operations, std::string> str_operations = {
    {Operations::Add, "Add"}, 
    {Operations::Equate, "Equate"}, 
    {Operations::Return, "Return"}, 
    {Operations::Jump, "Jump"}, 
    {Operations::Call, "Call"}, 
    {Operations::CallArg, "CallArg"}, 
    {Operations::Pop, "Pop"}, 
    {Operations::Print, "Print"},
};
std::ostream &operator<<(std::ostream &os, Operation const &o) { 
    switch (o.type) {
        case OperationTypes::NameLookup: return os << "[" <<o.operation << "]";
        case OperationTypes::Address: return os << "<" <<o.operation << ">";
        case OperationTypes::Empty: return os << "[ ]";
        case OperationTypes::Operation: return os << str_operations[static_cast<Operations>(o.operation)];
        case OperationTypes::Tuple: return os << "(" <<o.operation << ")";
        default: return os << "???";
    }
}

using namespace Symbols;
std::map<NonTerminals, std::string> str_non_terminals = {
    {NonTerminals::ROOT, "ROOT"}, {NonTerminals::E, "E"}, 
    {NonTerminals::T0, "T0"},  {NonTerminals::T1, "T1"}, {NonTerminals::T2, "T2"}, 
    {NonTerminals::T3, "T3"}, {NonTerminals::T4, "T4"},
    {NonTerminals::V, "V"}, 
    {NonTerminals::Program, "Program"},
    {NonTerminals::Expressions, "Expressions"},
    {NonTerminals::Expression, "Expression"},
    {NonTerminals::BlockBegin, "BlockBegin"}, {NonTerminals::Block, "Block"}, 
    {NonTerminals::FunctionDefinition, "FunctionDefinition"}, 
    {NonTerminals::FunctionCall, "FunctionCall"}, 
    {NonTerminals::Tuple, "Tuple"}, 
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
    if (v->GetLValue())os << "l";
    if (v->plain_tuple)os << "t";
    switch (v->type) {
        case ValueTypes::Default: return os << v->GetValue();
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
