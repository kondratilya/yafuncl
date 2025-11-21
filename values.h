#pragma once
#include <string>
#include <sstream>
#include <list>

class Context;

namespace Values {
    class Value;

    typedef double DefaultValueType;
    class Address {
        Context *context=NULL;
        public:
        Address(size_t position, Context *context=NULL);
        size_t position;
        Context *getContext(Context *fallback=NULL);
        void SetContext(Context *context);
    };
    typedef std::list<Value*> Tuple;

    enum class ValueTypes {
        Default,
        Addr,
        Tuple,
        Undefined
    };

    class Value {
        public:
        void *value;
        ValueTypes type;
        Context* context=NULL; 

        Value(DefaultValueType value);
        Value(Address value);
        Value(Tuple *value);
        Value();
        Value(Value* value);
        Value* Define(const DefaultValueType value);
        Value* Define(Address value);
        Value* Define(Tuple *value);
        Value* Define(const Value *value);
        bool IsDefined();
        DefaultValueType GetValue(Context* context);
        Address GetAddress();
        Value* SetAddressContext(Context* context);
        operator std::string() { 
            std::ostringstream os;
            switch (type) {
                case ValueTypes::Default: os << GetValue(NULL); break;
                case ValueTypes::Addr: os << "<" << GetAddress().position << ">"; break;
                case ValueTypes::Tuple: {
                    os << "(";
                    for (auto el: *(Tuple*)(value)) {
                        os << std::string(*el) << ", ";    
                    }
                    os << ")";
                } break;
                case ValueTypes::Undefined: os << "?"; break;
                default: os << "???"; break;
            }
            return os.str(); 
        }

    };
}
