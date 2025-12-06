#include <string>
#include <memory>
#include "values.h"
#include "context.h"

using namespace Values;
using namespace std::string_literals;

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
    this->value = new UndefinedValue();
}
Value::Value(Value* value) {            //clone value
    this->value = value->value->clone();
    this->type = value->type;
}
Value::~Value() {
    std::cout << "-";
    switch (type) {
        case ValueTypes::Undefined:break;
        case ValueTypes::Default:
            delete static_cast<DefaultValueType*>(value);
        break;
        case ValueTypes::Address:
            delete static_cast<AddressType*>(value);
        break;
        case ValueTypes::Tuple:
            delete static_cast<TupleType*>(value);
        break;
    }
}

Value* Value::Reference(VariableId id, Context* context) {
    reference_ = {static_cast<int>(id), context};
    return this;
}
Value* Value::Reference(Value* source_reference) {
    reference_ = source_reference->reference_;
    return this;
}
std::string Value::GetVariableName() const {
    if (IsReferenced())
        return reference_.context->GetVariableName(reference_.id);
    return "";
}
bool Value::IsReferenced() const {
    return reference_.id!=-1;
}

bool Value::IsMutable() const {
    if (!IsReferenced()) return false;
    if (type==ValueTypes::Undefined) return true;
    return reference_.context->IsVariableMutable(reference_.id);
}

Value *Value::SetTo(const Value* value) {               // Set internal value and type
    if (!IsMutable()) {
        throw std::runtime_error("Exec: Modifying immutable value \""s + GetVariableName() + "\"");
    }
    delete this->value;
    this->value = value->value->clone();
    this->type = value->type;
    return this;
}

AddressType& Value::GetAddress() const {
    return value->Address();
}

Value *Value::Calculate() {
    if (type==ValueTypes::Address)
        return GetAddress().Run()->Calculate();
    return this;
}

DefaultValueType& Value::GetValue() {
    return value->ToDefaultValueType();
}

TupleType& Value::GetTuple() {
    switch (type) {
        case ValueTypes::Undefined:
            return *(new TupleType());                /// !!! - how to delete??? !!!!
        case ValueTypes::Default: 
        case ValueTypes::Address:
            return *(new TupleType({new Value(this)}));              /// !!! - how to delete??? !!!!
        default:
            return *(TupleType*)value;
    }
}

bool Value::GetBool() {
    return value->ToBool();
}

Value::operator std::string() { 
    return std::string(*value);
}

std::string Value::str() { 
    return value->str();
}

void Value::Delete(Value *value) {
    if (!value->IsReferenced())
        delete value;
}

AddressType::AddressType(size_t position, Context *context){
    this->position=position;
    this->context_=context;
}

Value *AddressType::Run() {
    return (new Context(context_, this->position))->Run();
}

Value *AddressType::Run(Values::Value *arguments) {
    return (new Context(context_, this->position, arguments))->Run();
}

AbstractValue* AddressType::Calculate() { return Run()->value->Calculate(); };

AddressType::operator std::string() const { 
    std::ostringstream os; 
    os << "<" << this->position << ">";
    // if (!context)
    //     os << "<" << this->position << ">";
    // else 
    //     os << "<" << this->position << ":"<< context <<">";
    return os.str();
};

TupleType::TupleType(std::initializer_list<Value*> init/*, Value*value*/) : std::list<Value*>(init) {};

TupleType::operator std::string() const { 
    std::ostringstream os; 
    os << "(";
    for (auto el: *this) os << std::string(*el) << ", ";
    os << ")";
    return os.str();
};

std::string TupleType::str() { 
    std::ostringstream os; 
    for (auto el: *this) os << el->str(); 
    return os.str();
};

TupleType &TupleType::operator+ (TupleType &other) {
    other.splice(this->std::list<Value*>::end(), other);
    return *this;
}

Value *TupleType::operator[] (int index) {
    if (index < 0) index = this->size() + index;
    if (index < 0 || index >= this->size()) {
        throw std::runtime_error("Exec: Index " + std::to_string(index) + " out of range"); 
    }
    Values::TupleType::iterator it = this->begin();
    std::advance(it, index);
    return *it;
}
 
TupleType TupleType::Clone(const TupleType &src) {
    TupleType dest;
    for (auto element: src) {
        dest.push_back((new Value(element)));
    }
    return dest;
}