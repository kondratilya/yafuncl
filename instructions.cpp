#include "instructions.h"

#define STANDARD_BINARY_OPERATOR(_O_)  { \
                Values::DefaultValueType value2 = context->Pop()->GetValue(context); \
                Values::DefaultValueType value1 = context->Pop()->GetValue(context); \
                context->Push(new Values::Value(value1 _O_ value2)); \
            }

ReturnCode OperatorInstruction::Run(Context *context) {
    switch (op) {
        case Operators::Jump:
            context->Jump(context->Pop()->GetAddress().position);
            return ReturnCode::Continue;
        case Operators::Call: {
            Values::AddressType address = context->Pop()->GetAddress();
            context->Push(address.getContext(context)->Run());
        } break;
        case Operators::CallArg: {
            Values::AddressType address = context->Pop()->GetAddress();
            Values::Value *arguments = new Values::Value(context->Pop());
            context->Push(address.getContext(context)->WithArgument(arguments)->Run());
        } break;
        case Operators::Return:
            return ReturnCode::Return;
        case Operators::Pop:
            context->Result(new Values::Value(context->Pop()));
        break;
        case Operators::Equate: {
            Values::Value *v = context->Pop();
            context->Top()->SetTo(v);
        } break;
        case Operators::Print: 
            std::cout << "\"" << std::string(*context->Top()) << "\" ";
        break;
        case Operators::Add: STANDARD_BINARY_OPERATOR(+); break;
        case Operators::Substract: STANDARD_BINARY_OPERATOR(-); break;
        case Operators::Multiply: STANDARD_BINARY_OPERATOR(*); break;
        case Operators::Divide: STANDARD_BINARY_OPERATOR(/); break;
        case Operators::Mod: {
            Values::DefaultValueType value2 = context->Pop()->GetValue(context);
            Values::DefaultValueType value1 = context->Pop()->GetValue(context);
            context->Push(new Values::Value(value1 - (int)(value1/value2) * value2)); 
        } break;
        case Operators::Or: STANDARD_BINARY_OPERATOR(||); break;
        case Operators::And: STANDARD_BINARY_OPERATOR(&&); break;
        case Operators::IsEqual: STANDARD_BINARY_OPERATOR(==); break;
        case Operators::IsNotEqual: STANDARD_BINARY_OPERATOR(!=); break;
        case Operators::IsLess: STANDARD_BINARY_OPERATOR(<); break;
        case Operators::IsMore: STANDARD_BINARY_OPERATOR(>); break;
        case Operators::IsLessOrEqual: STANDARD_BINARY_OPERATOR(<=); break;
        case Operators::IsMoreOrEqual: STANDARD_BINARY_OPERATOR(>=); break;
        case Operators::Negative:
            context->Push(new Values::Value(-context->Pop()->GetValue(context)));
        break;
        case Operators::Positive:
            context->Push(new Values::Value(context->Pop()->GetValue(context)));
        break;
        case Operators::Inc: 
            context->Push(new Values::Value(context->Pop()->GetValue(context)+1));      //Неявное объявление 1
        break;
        case Operators::Dec: 
            context->Push(new Values::Value(context->Pop()->GetValue(context)-1));
        break;
        case Operators::Not:
            context->Push(new Values::Value(static_cast<Values::DefaultValueType>(!context->Pop()->GetValue(context))));
         break;

    }
    return ReturnCode::None;
}

ReturnCode VariableInstruction::Run(Context *context) {
   context->Push(context->GetVariable(index));
    return ReturnCode::None;
}

ReturnCode AddressInstruction::Run(Context *context) {
    context->Push(new Values::Value(Values::AddressType(position)));
    return ReturnCode::None;
}

ReturnCode TupleInstruction::Run(Context *context) {
    Values::TupleType value;
    for (int i=0;i<size;++i) {
        value.push_front(context->Pop());
    }
    context->Push(new Values::Value(value));
    return ReturnCode::None;
}
