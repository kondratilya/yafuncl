#include <iostream>
#include <map>
#include "operations.h"
#include "symbols.h"
#include "values.h"

std::map<Operations, std::string> str_operations = {
    {Operations::Add, "Add"}, 
    {Operations::Substract, "Substract"}, 
    {Operations::Multiply, "Multiply"}, 
    {Operations::Negative, "Negative"}, 
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
    {NonTerminals::Tuple, "Tuple"}, 
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
    /*if (v->GetLValue())os << "l";
    if (v->plain_tuple)os << "t";*/
    if (v->modifyers.count(Modifyers::Lvalue))os << "l";
    if (v->modifyers.count(Modifyers::LiteralTuple))os << "t";
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
