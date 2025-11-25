#pragma once
#include <string>
#include <sstream>
#include <list>
#include "variables_table.h"

class Context;

namespace Values {
    class Value;
    
    typedef double DefaultValueType;

    class AddressType {
        Context *context=NULL;
        public:
        AddressType(size_t position, Context *context=NULL);
        size_t position;
        Context *getContext(Context *fallback=NULL);
        void SetContext(Context *context);
        operator std::string() const;
    };

    class TupleType: public std::list<Value*> {
        public:
        TupleType() : std::list<Value*>() {}
        TupleType(std::initializer_list<Value*> init) : std::list<Value*>(init) {};
        operator std::string() const;
        std::string str(Context *context=NULL) const;
        static TupleType Clone(const TupleType &src);
    };
    
    enum class ValueTypes {
        Default,
        Address,
        Tuple,
        Undefined
    };

    class Value {
        struct {
            int id = -1;
            Context *context = NULL;
        } linked_to_variable_;
        public:
        void *value = NULL;
        ValueTypes type;
        Value(DefaultValueType value);
        Value(AddressType value);
        Value(TupleType value);
        Value();
        Value(Value* value);        // Clone
        ~Value();

        Value* LinkToVariable(VariableId id, Context* context);
        std::string GetVariableName();
        bool IsLinkedToVariable();

        Value* SetTo(const Value *value);  // Set internal value and type

        DefaultValueType& GetValue(Context* context=NULL) const;
        AddressType& GetAddress() const;
        TupleType& GetTuple(Context* context=NULL);

        operator std::string();
        std::string str(Context* context=NULL);

        static void Delete(Value *value);
    };
}
