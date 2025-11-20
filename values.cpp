#include <string>
#include "values.h"
#include "context.h"

Value::Value(DefaultValueType value) {
    Define(value);
    defined = true;
}
Value::Value(Address value) {
    Define(value);
    defined = true;
}
Value::Value(Tuple *value) {
    Define(value);
    defined = true;
}
Value::Value() {
    type = ValueTypes::Undefined;
    defined = true;
}
Value::Value(Value* value) {
    defined = true;
    this->value = value->value;
    this->type = value->type;
}
Value* Value::Define(const DefaultValueType value) {
    this->value = new DefaultValueType(value);
    type = ValueTypes::Default;
    defined = true;
    return this;
}
Value* Value::Define(const Address value) {
    this->value = new Address(value);
    type = ValueTypes::Address;
    defined = true;
    return this;
}
Value* Value::Define(Tuple *value) {
    this->value = value;
    type = ValueTypes::Tuple;
    defined = true;
    return this;
}
Value* Value::Define(const Value *value) {
    defined = true;
    if (!value) {
        type = ValueTypes::Undefined;
        return this;
    }
    this->value = value->value;
    this->type = value->type;
    return this;
}
bool Value::IsDefined() {
    return defined;
}
DefaultValueType Value::GetValue(Context* context) {
    if (!IsDefined()) 
        throw std::runtime_error("Exec: Operation with undefined value");
    if (type==ValueTypes::Undefined) {
        throw std::runtime_error("Exec: Operation with Null value"); 
    }
    if (type==ValueTypes::Address) {
        if (!context) 
            return (DefaultValueType)0;
        return (new Context(context, GetAddress()))->Run()->GetValue(context);
        //return context->Run()->GetValue(context, *(Address*)value);
    }
    return *(DefaultValueType*)value;
}
Address Value::GetAddress() {
    if (type!=ValueTypes::Address)
        throw std::runtime_error("Exec: Call to non-address");
    return *(Address*)value;
}
