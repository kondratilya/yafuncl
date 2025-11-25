#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include "code.h"
#include "modifyers.h"
#include "access_types.h"
#include "values.h"
#include "variables_table.h"
#include "context.h"

class Context;

enum class Operators {
    Pass,
    Add, Substract, Multiply, Divide, Mod,
    Negative, Positive, Dec, Inc, Not,
    Or, And, IsEqual, IsNotEqual, IsLess, IsMore, IsLessOrEqual, IsMoreOrEqual,
    Equate, Return, Jump, Jz, Jnz, Call, CallArg, Pop, 
    Unpack, Index, ToTuple, Length, Clone,
    Print, PrintLf, PrintChar, PrintMyName,
    InnerAccess,
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
            {Operators::Pass, "Pass"}, 
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
            {Operators::Unpack, "Unpack"}, 
            {Operators::Clone, "Clone"}, 
            {Operators::Index, "Index"}, 
            {Operators::ToTuple, "ToTuple"}, 
            {Operators::Length, "Length"}, 
            {Operators::Return, "Return"}, 
            {Operators::Jump, "Jump"}, 
            {Operators::Jz, "Jz"}, 
            {Operators::Jnz, "Jnz"}, 
            {Operators::Call, "Call"}, 
            {Operators::CallArg, "CallArg"}, 
            {Operators::InnerAccess, "InnerAccess"}, 
            {Operators::Pop, "Pop"}, 
            {Operators::Print, "Print"},
            {Operators::PrintLf, "\\n"},
            {Operators::PrintMyName, "Me!"},
        };   
        return str_operators[static_cast<Operators>(op)]; 
    }

    ReturnCode Run(Context *context) override;
};

class VariableInstruction: public Instruction {
    VariableId index;
    AccessType access_type_;
    bool is_const = true;
    public:
    VariableInstruction(VariableId index, Modifyers modifyers={}): Instruction() {
        this->index = index;
        access_type_ = AccessType::Default;
        if (modifyers.count(Modifyer::Inner) && modifyers.count(Modifyer::Outer)) {
            throw std::runtime_error("Exec: Conflicting modifyers");
        }
        if (modifyers.count(Modifyer::Inner)) {
            access_type_ = AccessType::Inner;
        } else if (modifyers.count(Modifyer::Outer)) {
            access_type_ = AccessType::Outer;
        }
    }
    operator std::string() const override { 
        std::ostringstream s; s << "[" << index << "]"; return s.str();
    }
    ReturnCode Run(Context *context) override;
};

class EmptyValueInstruction: public Instruction {
    public:
    EmptyValueInstruction(): Instruction() {
    }
    operator std::string() const override { 
        std::ostringstream s; s << "[]"; return s.str();
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
