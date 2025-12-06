#pragma once
#include <string>
#include <sstream>
#include <list>
#include "variables_table.h"

class Context;

namespace Values {
    class Value;
    class AddressType;
    class DefaultValueType;
    class TupleType;
    

    class AbstractValue {
        public:
        virtual AbstractValue* clone() const = 0;
        virtual AddressType& Address() { throw std::runtime_error("Exec: Call to non-address"); }

        virtual AbstractValue* Calculate() { return this; }

        virtual DefaultValueType &ToDefaultValueType() = 0;
        virtual bool ToBool() = 0;
        virtual operator std::string() const = 0;
        virtual std::string str() = 0;
        virtual TupleType *ToTupleType() = 0;

    };

    class DefaultValueType: public AbstractValue {
        double value_;
        public:
        DefaultValueType(double value): value_(value) {}
        operator std::string() const override { std::ostringstream os; os << value_; return os.str(); }
        std::string str() override { return std::string(1, (char)value_); }
        operator char() const { return (char)value_; }
        operator bool() const { return value_!=0; }
        operator int() const { return (int)value_; }
        operator double() const { return value_; }
        DefaultValueType operator+(DefaultValueType &other) const { return DefaultValueType(value_ + other.value_); }
        DefaultValueType operator-(DefaultValueType &other) const { return DefaultValueType(value_ - other.value_); }
        DefaultValueType operator*(DefaultValueType &other) const { return DefaultValueType(value_ * other.value_); }
        DefaultValueType operator/(DefaultValueType &other) const { return DefaultValueType(value_ / other.value_); }
        DefaultValueType operator%(DefaultValueType &other) const { return DefaultValueType( value_ - (int)(value_/other.value_) * other.value_ ); }
        DefaultValueType operator-() const { return DefaultValueType( -value_ ); }
        bool operator==(DefaultValueType &other) const { return value_ == other.value_; }
        bool operator!=(DefaultValueType &other) const { return value_ != other.value_; }
        bool operator<(DefaultValueType &other) const { return value_ < other.value_; }
        bool operator>(DefaultValueType &other) const { return value_ > other.value_; } 
        bool operator<=(DefaultValueType &other) const { return value_ <= other.value_; }
        bool operator>=(DefaultValueType &other) const { return value_ >= other.value_; } 
        DefaultValueType inc() const { return DefaultValueType( value_+1 ); }
        DefaultValueType dec() const { return DefaultValueType( value_-1 ); }

        AbstractValue* clone() const override { return new DefaultValueType(value_); }
        
        DefaultValueType &ToDefaultValueType() override { return *this; };
        bool ToBool() override { return static_cast<bool>(*this); };
        TupleType *ToTupleType() override { return nullptr; }
    };

    class AddressType: public AbstractValue  {
        public:
        Context *context_ = NULL;
        AddressType(size_t position, Context *context);
        size_t position;
        Value *Run();
        Value *Run(Values::Value *arguments);

        AbstractValue* clone() const override { return new AddressType(position, context_); }
        AddressType& Address() override { return *this; }

        AbstractValue* Calculate() override;

        DefaultValueType &ToDefaultValueType() override { return Calculate()->ToDefaultValueType(); };
        bool ToBool() override { return Calculate()->ToBool(); }

        operator std::string() const override;
        std::string str() override {  return Calculate()->str(); }

        TupleType *ToTupleType() override { return nullptr; }
    };

    class TupleType: public AbstractValue, public std::list<Value*> {
        
        public:
        TupleType() : std::list<Value*>() {}
        TupleType(std::initializer_list<Value*> init);
        TupleType(std::list<Value*> init) : std::list<Value*>(init) {}
        TupleType &operator+ (TupleType &other);
        Value *operator[] (int index); 
        operator std::string() const override;
        std::string str() override;
        static TupleType Clone(const TupleType &src);
        AbstractValue* clone() const override { return new TupleType(*this); }

        DefaultValueType &ToDefaultValueType() override { throw std::runtime_error("Exec: Cannot onvert Tuple to Value"); };
        bool ToBool() override { return size()!=0; }
        TupleType *ToTupleType() override { return this; }
    };
    
    class UndefinedValue: public AbstractValue {
        public:
        AbstractValue* clone() const override { return new UndefinedValue(); }

        operator std::string() const override { return "?"; };
        std::string str() override { return this->operator std::string(); };

        DefaultValueType &ToDefaultValueType() override { throw std::runtime_error("Exec: Operation with Undefined value");  };
        bool ToBool() override { return false; }
        TupleType *ToTupleType() override { return nullptr; }
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
        } reference_;
        

        public:
        AbstractValue *value;
        ValueTypes type;
       
        Value(DefaultValueType value);
        Value(AddressType value);
        Value(TupleType value);
        Value();
        Value(Value* value);        // Clone
        ~Value();

        Value* Reference(VariableId id, Context* context);
        Value* Reference(Value* source_reference);
        std::string GetVariableName() const;
        bool IsReferenced() const;
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
