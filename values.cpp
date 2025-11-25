#include <string>
#include <memory>
#include "values.h"
#include "context.h"

using namespace Values;

Value::Value(DefaultValueType value) {
    this->value = new DefaultValueType(value);
    type = ValueTypes::Default;
}
Value::Value(AddressType value) {
    this->value = new AddressType(value);
    type = ValueTypes::Address;
}
Value::Value(TupleType value) {
    this->value = new TupleType(value);
    type = ValueTypes::Tuple;
}
Value::Value() {
    type = ValueTypes::Undefined;
}
Value::Value(Value* value) {            //clone value
    SetTo(value);
}
Value *Value::SetTo(const Value* value) {               // Set internal value and type
    this->value = value->value;
    this->type = value->type;
    return this;
}

DefaultValueType& Value::GetValue(Context* context) const {
    switch (type) {
        case ValueTypes::Undefined:
            throw std::runtime_error("Exec: Operation with Undefined value"); 
        case ValueTypes::Address:
            return GetAddress().getContext(context)->Run()->GetValue(context);
        case ValueTypes::Tuple:
            throw std::runtime_error("Exec: Conversion Tuple -> Value"); 
        default:
            return *(DefaultValueType*)value;
    }
}

AddressType& Value::GetAddress() const {
    if (type!=ValueTypes::Address)
        throw std::runtime_error("Exec: Call to non-address");
    return *(AddressType*)value;
}

TupleType& Value::GetTuple(Context* context) {
    switch (type) {
        case ValueTypes::Undefined:
            return *(new TupleType());                /// !!! - how to delete??? !!!!
        case ValueTypes::Default: 
        case ValueTypes::Address:
            return *(new TupleType{this});              /// !!! - how to delete??? !!!!
        default:
            return *(TupleType*)value;
    }
}

Value::operator std::string() { 
    std::ostringstream os;
    switch (type) {
        case ValueTypes::Default: os << GetValue(); break;
        case ValueTypes::Address: os << std::string(GetAddress()); break;
        case ValueTypes::Tuple: os << std::string(GetTuple()) ; break;
        case ValueTypes::Undefined: os << "?"; break;
        default: os << "???"; break;
    }
    // os << ":" << value;
    return os.str(); 
}

std::string Value::str(Context* context) { 
    std::ostringstream os;
    switch (type) {
        case ValueTypes::Default: 
        case ValueTypes::Address: 
            os << (char)(GetValue(context)); break;
        case ValueTypes::Tuple: 
            os << GetTuple(context).str(context);
        break;
        case ValueTypes::Undefined: os << " "; break;
        default: os << "?"; break;
    }
    return os.str(); 
}

AddressType::AddressType(size_t position, Context *context){
    this->position=position;
    this->context=context;
}

Context *AddressType::getContext(Context *fallback) {
    // Если у адреса есть контекст, значит из него уже вышли через Return, можно делать Jump
    if (this->context) {
        this->context->Result()->Jump(this->position); // Этот Jump будет при вызове Return контекста. Тут только для наглядности
        return this->context;
    }
    return new Context(fallback, this->position);
}

void AddressType::SetContext(Context *context) {
    this->context = context;
}

AddressType::operator std::string() const { 
    std::ostringstream os; 
    os << "<" << this->position << ">";
    // if (!context)
    //     os << "<" << this->position << ">";
    // else 
    //     os << "<" << this->position << ":"<< context <<">";
    return os.str();
};

TupleType::operator std::string() const { 
    std::ostringstream os; 
    os << "(";
    for (auto el: *this) os << std::string(*el) << ", ";
    os << ")";
    return os.str();
};

std::string TupleType::str(Context *context) const { 
    std::ostringstream os; 
    for (auto el: *this) os << el->str(context); 
    return os.str();
};

TupleType TupleType::Clone(const TupleType &src) {
    TupleType dest;
    for (auto element: src) {
        dest.push_back(new Value(element));
    }
    return dest;
}