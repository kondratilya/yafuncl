#include <string>
#include "values.h"
#include "context.h"

using namespace Values;

Value::Value(DefaultValueType value) {
    Define(value);
}
Value::Value(Address value) {
    Define(value);
}
Value::Value(Tuple *value) {
    Define(value);
}
Value::Value() {
    type = ValueTypes::Undefined;
}
Value::Value(Value* value) {
    this->value = value->value;
    this->type = value->type;
}
Value* Value::Define(const DefaultValueType value) {
    this->value = new DefaultValueType(value);
    type = ValueTypes::Default;
    return this;
}
Value* Value::Define(Address value) {
    this->value = new Address(value);
    type = ValueTypes::Addr;
    return this;
}
Value* Value::Define(Tuple *value) {
    this->value = value;
    type = ValueTypes::Tuple;
    return this;
}
Value* Value::Define(const Value *value) {
    if (!value) {
        type = ValueTypes::Undefined;
        return this;
    }
    this->value = value->value;
    this->type = value->type;
    return this;
}
DefaultValueType Value::GetValue(Context* context) {
    if (type==ValueTypes::Undefined) {
        throw std::runtime_error("Exec: Operation with Undefined value"); 
    }
    if (type==ValueTypes::Addr) {
        // if (!context) 
        //     return (DefaultValueType)0;
        // return (new Context(context, GetAddress()))->Run()->GetValue(context);

        return GetAddress().getContext(context)->Run()->GetValue(context);
    }
    return *(DefaultValueType*)value;
}

Address::Address(size_t position, Context *context){
    this->position=position;
    this->context=context;
}
Address Value::GetAddress() {
    if (type!=ValueTypes::Addr)
        throw std::runtime_error("Exec: Call to non-address");
    return *(Address*)value;
}

Value* Value::SetAddressContext(Context* context) {
    if (type!=ValueTypes::Addr)
        throw std::runtime_error("Exec: ??? Set context for non-address");  
    ((Address*)value)->SetContext(context);
    return this;
}

Context *Address::getContext(Context *fallback) {
    // Если у адреса есть контекст, значит из него уже вышли через Return, можно делать Jump
    if (this->context) {
        this->context->Jump(this->position); // Этот Jump будет при вызове Return контекста. Тут только для наглядности
        return this->context;
    }
    return new Context(fallback, this->position);
}

void Address::SetContext(Context *context) {
    this->context = context;
}

// std::ostream &operator<<(std::ostream &os, Value *v) {
//     switch (v->type) {
//         case ValueTypes::Default: return os << v->GetValue(NULL);
//         case ValueTypes::Address: return os << "<" << v->GetAddress() << ">";
//         case ValueTypes::Tuple: {
//             os << "(";
//             for (auto el: *(Tuple*)(v->value)) {
//                 os << el << ", ";    
//             }
//             os << ")";
//         } return os;
//         case ValueTypes::Undefined: return os << "?";
//         default: return os << "???";
//     }
// }
