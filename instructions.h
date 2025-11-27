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
    UseInnerAccess, UseMutableVars,
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
    static std::map<Operators, std::string> str_operators;
    Operators op;
    public:
    OperatorInstruction(Operators op): Instruction() {
        this->op = op;
    }
    operator std::string() const override { 
        return str_operators[op]; 
    }

    ReturnCode Run(Context *context) override;
};

class VariableInstruction: public Instruction {
    VariableId index;
    AccessType access_type_ = AccessType::Default;
    MutabilityType mutability_type_ = MutabilityType::Default;
    public:
    VariableInstruction(VariableId index, Modifyers modifyers={}): Instruction() {
        this->index = index;
        if (modifyers.count(Modifyer::Inner) && modifyers.count(Modifyer::Outer)) {
            throw std::runtime_error("Exec: Conflicting modifyers: inner, outer");
        }
        if (modifyers.count(Modifyer::Inner)) {
            access_type_ = AccessType::Inner;
        } else if (modifyers.count(Modifyer::Outer)) {
            access_type_ = AccessType::Outer;
        }
        if (modifyers.count(Modifyer::Mutable) && modifyers.count(Modifyer::Immutable) ) {
            throw std::runtime_error("Exec: Conflicting modifyers: mutable, immutable");
        }
        if (modifyers.count(Modifyer::Mutable)) {
            mutability_type_ = MutabilityType::Mutable;
        } else if (modifyers.count(Modifyer::Immutable)) {
            mutability_type_ = MutabilityType::Immutable;
        }
    }
    operator std::string() const override { 
        std::ostringstream s; 
        s << "[" << 
            ((access_type_ == AccessType::Inner)?"inner ":"") << 
            ((access_type_ == AccessType::Outer)?"outer ":"") << 
            ((mutability_type_ == MutabilityType::Mutable)?"mutable ":"") << 
            ((mutability_type_ == MutabilityType::Immutable)?"immutable ":"") << index << "]"; 
        return s.str();
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
