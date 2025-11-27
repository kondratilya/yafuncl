#include <iterator>
#include "instructions.h"

#define STANDARD_BINARY_OPERATOR(_O_)  { \
                Values::Value *value2 = context->Pop(); \
                Values::Value *value1 = context->Pop(); \
                context->Push(new Values::Value(value1->GetValue(context) _O_ value2->GetValue(context))); \
            }

ReturnCode OperatorInstruction::Run(Context *context) {
    switch (op) {
        case Operators::UseInnerAccess:
            context->SetInnerAccess();
        break;
        case Operators::UseMutableVars:
            context->SetMutableVars();
        break;
        case Operators::Jump:
            context->Jump(context->Pop()->GetAddress());
            return ReturnCode::Continue;
        case Operators::Jz: {
            Values::AddressType address = context->Pop()->GetAddress();
            Values::DefaultValueType condition = context->Pop()->GetBool(context);
            if (!condition) {
                context->Jump(address);
                return ReturnCode::Continue;
            }
        } break;
        case Operators::Jnz: {
            Values::AddressType address = context->Pop()->GetAddress();
            Values::DefaultValueType condition = context->Pop()->GetBool(context);
            if (condition) {
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
        case Operators::Clone: 
            context->Push(new Values::Value(Values::TupleType::Clone(context->Pop()->GetTuple(context))));
        break;
        case Operators::Unpack: {
            Values::TupleType value = context->Pop()->GetTuple(context);
            Values::TupleType dest = context->Top()->GetTuple(context);
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
        case Operators::Index: {
            Values::DefaultValueType index = context->Pop()->GetValue(context);
            Values::TupleType tuple = context->Pop()->GetTuple(context);
            if (index < 0) index = tuple.size() + index;
            if (index < 0 || index > tuple.size()) {
                throw std::runtime_error("Exec: Index out of range"); 
            }
            Values::TupleType::iterator it = tuple.begin();
            std::advance(it, index);
            context->Push(*it);
        } break;
        case Operators::Length: 
            context->Push(new Values::Value(context->Pop()->GetTuple(context).size()));
        break;
        case Operators::ToTuple: 
            context->Push(new Values::Value(context->Pop()->GetTuple(context)));
        break;
        case Operators::Print: 
            std::cout << "\"" << std::string(*context->Top()) << "\" ";
        break;
        case Operators::PrintChar: 
            std::cout << context->Top()->str(context);
        break;
        case Operators::PrintMyName: 
            std::cout << context->Top()->GetVariableName() << " ";
        break;
        case Operators::PrintLf: 
            std::cout << std::endl;
        break;
        case Operators::Add: {
            Values::Value *value2 = context->Pop()->Calculate(context);
            Values::Value *value1 = context->Pop()->Calculate(context);
            if (value1->type == Values::ValueTypes::Tuple || value2->type == Values::ValueTypes::Tuple) {
                context->Push(new Values::Value(value1->GetTuple(context) + value2->GetTuple(context))); 
            } else {
                context->Push(new Values::Value(value1->GetValue(context) + value2->GetValue(context)));
            }
        } break;
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
            Values::Value *value1 = context->Pop();
            if (value1->GetBool(context)) {
                context->Push(new Values::Value(value1)); 
                break;
            }
            if (value2->GetBool(context)) {
                context->Push(new Values::Value(value2)); 
                break;
            }
            context->Push(new Values::Value(value2));
        } break;
        case Operators::And: {
            Values::Value *value2 = context->Pop(); 
            Values::Value *value1 = context->Pop(); 
            context->Push(new Values::Value(value1->GetBool(context) && value2->GetBool(context))); 
        } break;
        case Operators::IsEqual: {
            Values::Value *value2 = context->Pop()->Calculate(context);
            Values::Value *value1 = context->Pop()->Calculate(context);
            if (value1->type == Values::ValueTypes::Undefined || value2->type == Values::ValueTypes::Undefined) {
                context->Push(new Values::Value());
                break;
            }
            context->Push(new Values::Value(value1->GetBool(context) == value2->GetBool(context))); 
        } break;
        case Operators::IsNotEqual:  {
            Values::Value *value2 = context->Pop()->Calculate(context);
            Values::Value *value1 = context->Pop()->Calculate(context);
            if (value1->type == Values::ValueTypes::Undefined || value2->type == Values::ValueTypes::Undefined) {
                context->Push(new Values::Value());
                break;
            }
            context->Push(new Values::Value(value1->GetBool(context) != value2->GetBool(context))); 
        } break;
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
    context->Push(context->GetVariable(index, access_type_, mutability_type_));
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
