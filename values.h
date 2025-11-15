#pragma once
#include <string>
#include <list>

class Value;

typedef std::string DefaultValueType;
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
    bool defined;
    bool lvalue=false;
    bool plain_tuple=false;
    ValueTypes type;
    public:
    bool GetLValue() { return lvalue; }
    void SetLValue(bool lvalue) { this->lvalue=lvalue; }

    Value(DefaultValueType value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Address value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Tuple *value, bool defined=true) {
        Define(value);
        this->defined=defined;
    }
    Value(Value *value) {
        this->value = value->value;
        this->defined = value->defined;
        this->type = value->type;
        this->lvalue = value->lvalue;
    }
    Value() {
        this->defined=false;
        type = ValueTypes::Undefined;
    }
    void Define(const DefaultValueType value) {
        this->value = new DefaultValueType(value);
        type = ValueTypes::Default;
        defined = true;
        plain_tuple = false;
    }
    void Define(const Address value) {
        this->value = new Address(value);
        type = ValueTypes::Address;
        defined = true;
        plain_tuple = false;
    }
    void Define(Tuple *value, bool plain=true) {
        this->value = value;
        type = ValueTypes::Tuple;
        defined = true;
        plain_tuple = plain;
    }
    void Define(const Value *value) {
        this->value = value->value;
        this->defined = value->defined;
        this->type = value->type;
        this->lvalue = value->lvalue;
        plain_tuple = false;
    }
    DefaultValueType GetValue() {
        return *(DefaultValueType*)value;
    }
    Address GetAddress() {
        if (type!=ValueTypes::Address)
            throw std::runtime_error("Exec: Call to non-address");
        return *(Address*)value;
    }
    friend std::ostream &operator<<(std::ostream &os, Value *v);
};
std::ostream &operator<<(std::ostream &os, Value *v);