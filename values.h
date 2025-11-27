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
        public:
        Context *context_ = NULL;
        AddressType(size_t position, Context *context);
        size_t position;
        Value *Run();
        Value *Run(Values::Value *arguments);
        operator std::string() const;
    };

    class TupleType: public std::list<Value*> {
        public:
        TupleType() : std::list<Value*>() {}
        TupleType(std::initializer_list<Value*> init);
        TupleType &operator+ (TupleType &other);
        Value *operator[] (int index); 
        operator std::string() const;
        std::string str() const;
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
        void *value = NULL;

        public:
        ValueTypes type;
       
        Value(DefaultValueType value);
        Value(AddressType value);
        Value(TupleType value);
        Value();
        Value(Value* value);        // Clone
        ~Value();

        Value* LinkToVariable(VariableId id, Context* context);
        Value* LinkToVariable(Value* source_link);
        std::string GetVariableName() const;
        bool IsLinkedToVariable() const;
        bool IsMutable() const;

        Value* SetTo(const Value *value);  // Set internal value and type

        Value *Calculate();
        DefaultValueType& GetValue();
        AddressType& GetAddress() const;
        TupleType& GetTuple();
        bool GetBool();

        operator std::string();
        std::string str();

        static void Delete(Value *value);
    };
}
