#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include "code.h"
#include "values.h"
#include "variables_table.h"
#include "context.h"

class Context;

enum class Operators {
    Add, Substract, Multiply, Divide, Mod,
    Negative, Positive, Dec, Inc, Not,
    Or, And, IsEqual, IsNotEqual, IsLess, IsMore, IsLessOrEqual, IsMoreOrEqual,
    Equate, Return, Jump, Call, CallArg, Pop, Print, EquateTuple,
};

enum class ReturnCode {
    None, Return, Continue
};

class Instruction {
    public:
    Instruction() {};
    virtual ReturnCode Run(Context *context) { return ReturnCode::None; };
    virtual operator std::string() const { return "???"; }
};

class OperatorInstruction: public Instruction {
    Operators op;
    public:
    OperatorInstruction(Operators op): Instruction() {
        this->op = op;
    }
    operator std::string() const override { 
        std::map<Operators, std::string> str_operators = {
            {Operators::Add, "+"}, 
            {Operators::Substract, "-"}, 
            {Operators::Multiply, "*"}, 
            {Operators::Divide, "/"},
            {Operators::Mod, "%"},
            {Operators::Negative, "Negative"}, 
            {Operators::Positive, "Positive"}, 
            {Operators::Dec, "Dec"}, 
            {Operators::Inc, "Inc"}, 
            {Operators::Not, "Not"}, 
            {Operators::Or, "||"}, 
            {Operators::And, "&&"}, 
            {Operators::IsEqual, "=="}, 
            {Operators::IsNotEqual, "!="}, 
            {Operators::IsLess, "<"}, 
            {Operators::IsMore, ">"}, 
            {Operators::IsLessOrEqual, "<="}, 
            {Operators::IsMoreOrEqual, ">="}, 
            {Operators::Equate, "Equate"}, 
            {Operators::EquateTuple, "EquateTuple"}, 
            {Operators::Return, "Return"}, 
            {Operators::Jump, "Jump"}, 
            {Operators::Call, "Call"}, 
            {Operators::CallArg, "CallArg"}, 
            {Operators::Pop, "Pop"}, 
            {Operators::Print, "Print"},
        };   
        return str_operators[static_cast<Operators>(op)]; 
    }

    ReturnCode Run(Context *context) override;
};

class VariableInstruction: public Instruction {
    VariableId index;
    VariablesLookup *names_lookup_table;
    public:
    VariableInstruction(VariableId index, VariablesLookup *names_lookup_table=NULL): Instruction() {
        this->index = index;
        this->names_lookup_table = names_lookup_table;
    }
    operator std::string() const override { 
        std::ostringstream s; s << "[" << index << "]"; return s.str();
    }
    ReturnCode Run(Context *context) override;
};

class AddressInstruction: public Instruction {
    size_t position;
        public:
    AddressInstruction(size_t position): Instruction() {
        this->position = position;
    }
    operator std::string() const override { 
        std::ostringstream s; s << "<" << position << ">"; return s.str();
    }
    ReturnCode Run(Context *context) override;
};

class TupleInstruction: public Instruction {
    size_t size;
    public:
    TupleInstruction(size_t size): Instruction() {
        this->size = size;
    }
    operator std::string() const override { 
        std::ostringstream s; s << "(" << size << ")"; return s.str(); 
    }
    ReturnCode Run(Context *context) override;
};
