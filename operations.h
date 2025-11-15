#pragma once
#include <vector>

enum class OperationTypes {
    Empty,
    Operation,
    NameLookup,
    Address,
    Tuple,
};

enum class Operations {
    Add, Equate, Return, Jump, Call, CallArg, Pop, Print
};

class Operation {
    public:
    size_t operation;
    OperationTypes type;
    Operation (OperationTypes type, size_t value) {
        this->type = type;
        operation = value;
    }
    Operation (Operations value) {
        this->type = OperationTypes::Operation;
        operation = static_cast<size_t>(value);
    }
    Operation () {
        this->type = OperationTypes::Empty;
        operation = 0;
    }
};

std::ostream &operator<<(std::ostream &os, Operation const &o);

typedef std::vector<Operation*> Code;