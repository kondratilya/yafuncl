#pragma once
#include <string>
#include <list>
#include <set>

class Value;

typedef std::string DefaultValueType;
typedef size_t Address;
typedef std::list<Value*> Tuple;

enum class Modifyers {
    New,
    Lvalue,
    Defined,
    LiteralTuple,
};

enum class ValueTypes {
    Default,
    Address,
    Tuple,
    Undefined
};

class Value {
    void *value;
    ValueTypes type;
    std::set<Modifyers> modifyers;
    public:
    // bool GetLValue() { return lvalue; }
    // void SetLValue(bool lvalue) { this->lvalue=lvalue; }

    Value(DefaultValueType value, std::set<Modifyers> modifyers={}) {
        Define(value)->SetModifyers(modifyers);
        // this->defined=defined;
    }
    Value(Address value, std::set<Modifyers> modifyers={}/*, bool defined=true*/) {
        Define(value)->SetModifyers(modifyers);
        // this->defined=defined;
    }
    Value(Tuple *value, std::set<Modifyers> modifyers={}/*, bool defined=true*/) {
        Define(value)->SetModifyers(modifyers);
        // this->defined=defined;
    }
    // Value(Value *value, std::set<Modifyers> modifyers={}) {
    //     this->value = value->value;
    //     // this->defined = value->defined;
    //     this->type = value->type;
    //     SetModifyers(modifyers);
    //     // this->lvalue = value->lvalue;
    // }
    Value(std::set<Modifyers> modifyers={}) {
        // this->defined=false;
        type = ValueTypes::Undefined;
        SetModifyers(modifyers);
    }
    Value* Define(const DefaultValueType value) {
        this->value = new DefaultValueType(value);
        type = ValueTypes::Default;
        SetModifyers({Modifyers::Defined});
        RemoveModifyers({Modifyers::LiteralTuple, Modifyers::Lvalue});
        return this;
    }
    Value* Define(const Address value) {
        this->value = new Address(value);
        type = ValueTypes::Address;
        SetModifyers({Modifyers::Defined});
        RemoveModifyers({Modifyers::LiteralTuple, Modifyers::Lvalue});
        return this;
    }
    Value* Define(Tuple *value/*, bool plain=true*/) {
        this->value = value;
        type = ValueTypes::Tuple;
        SetModifyers({Modifyers::Defined});
        RemoveModifyers({Modifyers::LiteralTuple, Modifyers::Lvalue});
        return this;
    }
    Value* Define(const Value *value) {
        if (!value) {
            type = ValueTypes::Undefined;
            RemoveModifyers({Modifyers::Defined});
            return this;
        }
        this->value = value->value;
        // this->defined = value->defined;
        this->type = value->type;
        SetModifyers(value->modifyers);
        RemoveModifyers({Modifyers::LiteralTuple, Modifyers::Lvalue});
        // this->lvalue = value->lvalue;
        // plain_tuple = false;
        return this;
    }
    Value* SetModifyers(std::set<Modifyers> modifyers) {
        this->modifyers.insert(modifyers.begin(), modifyers.end());
        return this;
    }
    Value* RemoveModifyers(std::set<Modifyers> modifyers) {
        for (auto m : modifyers) {
            this->modifyers.erase(m);
        }
        return this;
    }
    bool Modifyer(const Modifyers modifyer) {
        return modifyers.count(modifyer);
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