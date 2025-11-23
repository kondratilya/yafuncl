#include <iterator>
#include "instructions.h"

#define STANDARD_BINARY_OPERATOR(_O_)  { \
                Values::DefaultValueType value2 = context->Pop()->GetValue(context); \
                Values::DefaultValueType value1 = context->Pop()->GetValue(context); \
                context->Push(new Values::Value(value1 _O_ value2)); \
            }

ReturnCode OperatorInstruction::Run(Context *context) {
    switch (op) {
        case Operators::Jump:
            context->Jump(context->Pop()->GetAddress());
            return ReturnCode::Continue;
        case Operators::Jz: {
            Values::AddressType address = context->Pop()->GetAddress();
            Values::DefaultValueType condition = context->Pop()->GetValue(context);
            if (!condition) {
                context->Jump(address);
                return ReturnCode::Continue;
            }
        } break;
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
            Values::Value *value = context->Pop();
            Values::Value *variable = context->Top();
            variable->SetTo(value);
        } break;
         case Operators::Unpack: {
            Values::TupleType value = context->Pop()->GetTuple();
            Values::TupleType dest = context->Top()->GetTuple();
            auto value_it = value.begin();
            auto dest_it = dest.begin();
            while (dest_it!=dest.end()) {
                if (value_it!=value.end()) {
                    (*dest_it)->SetTo(*value_it);
                    std::advance(value_it, 1);
                } else {
                    Values::Value empy = Values::Value();
                    (*dest_it)->SetTo(&empy); 
                }
                std::advance(dest_it, 1);
            }
        } break;
        case Operators::Print: 
            std::cout << "\"" << std::string(*context->Top()) << "\" ";
        break;
        case Operators::PrintChar: 
            std::cout << context->Top()->str(context);
        break;
        case Operators::PrintLf: 
            std::cout << std::endl;
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
        case Operators::Or: {
            Values::Value *value2 = context->Pop();
            Values::DefaultValueType value1value = context->Pop()->GetValue(context);
            if (value1value) {
                context->Push(new Values::Value(value1value)); 
                break;
            }
            Values::DefaultValueType value2value = value2->GetValue(context);
            if (value2) {
                context->Push(new Values::Value(value2value)); 
                break;
            }
            context->Push(new Values::Value(value1value));
        } break;
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
            context->Push(new Values::Value(!context->Pop()->GetValue(context)));
        break;
    }
    return ReturnCode::None;
}

ReturnCode VariableInstruction::Run(Context *context) {
   context->Push(context->GetVariable(index));
    return ReturnCode::None;
}

ReturnCode EmptyValueInstruction::Run(Context *context) {
    context->Push(new Values::Value());
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
