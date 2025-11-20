#pragma once
#include <string>
#include <list>

class Context;

class Value;

typedef double DefaultValueType;
typedef size_t Address;
typedef std::list<Value*> Tuple;

enum class ValueTypes {
    Default,
    Address,
    Tuple,
    Undefined
};

class Value {
    void *value;
    ValueTypes type;
    bool defined;
    public:

    Value(DefaultValueType value);
    Value(Address value);
    Value(Tuple *value);
    Value();
    Value(Value* value);
    Value* Define(const DefaultValueType value);
    Value* Define(const Address value);
    Value* Define(Tuple *value);
    Value* Define(const Value *value);
    bool IsDefined();
    DefaultValueType GetValue(Context* context);
    Address GetAddress();
    friend std::ostream &operator<<(std::ostream &os, Value *v);
};
std::ostream &operator<<(std::ostream &os, Value *v);