#include <iterator>
#include "instructions.h"

using namespace std::string_literals;

#define STANDARD_BINARY_OPERATOR(_O_)  { \
                Values::Value *value2 = context->Pop(); \
                Values::Value *value1 = context->Pop(); \
                context->Push(new Values::Value(value1->GetValue() _O_ value2->GetValue())); \
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
            Values::DefaultValueType condition = context->Pop()->GetBool();
            if (!condition) {
                context->Jump(address);
                return ReturnCode::Continue;
            }
        } break;
        case Operators::Jnz: {
            Values::AddressType address = context->Pop()->GetAddress();
            Values::DefaultValueType condition = context->Pop()->GetBool();
            if (condition) {
                context->Jump(address);
                return ReturnCode::Continue;
            }
        } break;
        case Operators::Call: {
            Values::AddressType address = context->Pop()->GetAddress();
            context->Push(address.Run());
        } break;
        case Operators::CallArg: {
            Values::AddressType address = context->Pop()->GetAddress();
            Values::Value *arguments = new Values::Value(context->Pop());
            context->Push(address.Run(arguments));
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
            context->Push(new Values::Value(Values::TupleType::Clone(context->Pop()->GetTuple())));
        break;
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
        case Operators::Index: {
            Values::DefaultValueType index = context->Pop()->GetValue();
            Values::Value* tuple_value = context->Pop();
            Values::TupleType tuple = tuple_value->GetTuple();
            context->Push(tuple[index]->LinkToVariable(tuple_value));
        } break;
        case Operators::Length: 
            context->Push(new Values::Value(context->Pop()->GetTuple().size()));
        break;
        case Operators::ToTuple: 
            context->Push(new Values::Value(context->Pop()->GetTuple()));
        break;
        case Operators::Print: 
            context->Output("'"s + std::string(*context->Top()) + "' "s);
        break;
        case Operators::PrintChar: 
            context->Output(context->Top()->str());
        break;
        case Operators::PrintMyName: 
            context->Output(context->Top()->GetVariableName() + " "s);
        break;
        case Operators::PrintLf: 
            context->Output("\n");
        break;
        case Operators::Add: {
            Values::Value *value2 = context->Pop()->Calculate();
            Values::Value *value1 = context->Pop()->Calculate();
            if (value1->type == Values::ValueTypes::Tuple || value2->type == Values::ValueTypes::Tuple) {
                context->Push(new Values::Value(value1->GetTuple() + value2->GetTuple())); 
            } else {
                context->Push(new Values::Value(value1->GetValue() + value2->GetValue()));
            }
        } break;
        case Operators::Substract: STANDARD_BINARY_OPERATOR(-); break;
        case Operators::Multiply: STANDARD_BINARY_OPERATOR(*); break;
        case Operators::Divide: STANDARD_BINARY_OPERATOR(/); break;
        case Operators::Mod: {
            Values::DefaultValueType value2 = context->Pop()->GetValue();
            Values::DefaultValueType value1 = context->Pop()->GetValue();
            context->Push(new Values::Value(value1 - (int)(value1/value2) * value2)); 
        } break;
        case Operators::Or: {
            Values::Value *value2 = context->Pop();
            Values::Value *value1 = context->Pop();
            if (value1->GetBool()) {
                context->Push(new Values::Value(value1)); 
                break;
            }
            if (value2->GetBool()) {
                context->Push(new Values::Value(value2)); 
                break;
            }
            context->Push(new Values::Value(value2));
        } break;
        case Operators::And: {
            Values::Value *value2 = context->Pop(); 
            Values::Value *value1 = context->Pop(); 
            context->Push(new Values::Value(value1->GetBool() && value2->GetBool())); 
        } break;
        case Operators::IsEqual: {
            Values::Value *value2 = context->Pop()->Calculate();
            Values::Value *value1 = context->Pop()->Calculate();
            if (value1->type == Values::ValueTypes::Undefined || value2->type == Values::ValueTypes::Undefined) {
                context->Push(new Values::Value());
                break;
            }
            context->Push(new Values::Value(value1->GetValue() == value2->GetValue())); 
        } break;
        case Operators::IsNotEqual:  {
            Values::Value *value2 = context->Pop()->Calculate();
            Values::Value *value1 = context->Pop()->Calculate();
            if (value1->type == Values::ValueTypes::Undefined || value2->type == Values::ValueTypes::Undefined) {
                context->Push(new Values::Value());
                break;
            }
            context->Push(new Values::Value(value1->GetValue() != value2->GetValue())); 
        } break;
        case Operators::IsLess: STANDARD_BINARY_OPERATOR(<); break;
        case Operators::IsMore: STANDARD_BINARY_OPERATOR(>); break;
        case Operators::IsLessOrEqual: STANDARD_BINARY_OPERATOR(<=); break;
        case Operators::IsMoreOrEqual: STANDARD_BINARY_OPERATOR(>=); break;
        case Operators::Negative:
            context->Push(new Values::Value(-context->Pop()->GetValue()));
        break;
        case Operators::Positive:
            context->Push(new Values::Value(context->Pop()->GetValue()));
        break;
        case Operators::Inc: 
            context->Push(new Values::Value(context->Pop()->GetValue()+1));      //Неявное объявление 1
        break;
        case Operators::Dec: 
            context->Push(new Values::Value(context->Pop()->GetValue()-1));
        break;
        case Operators::Not:
            context->Push(new Values::Value(!context->Pop()->GetBool()));
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
    context->Push(new Values::Value(Values::AddressType(position, context)));
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



std::map<Operators, std::string> OperatorInstruction::str_operators = {
    {Operators::Pass, "Pass"}, 
    {Operators::Add, "+"}, 
    {Operators::Substract, "-"}, 
    {Operators::Multiply, "*"}, 
    {Operators::Divide, "/"},
    {Operators::Mod, "%"},
    {Operators::Negative, "Negative"}, 
    {Operators::Positive, "Positive"}, 
    {Operators::Dec, "Dec"}, 
    {Operators::Inc, "Inc"}, 
    {Operators::Not, "Not"}, 
    {Operators::Or, "||"}, 
    {Operators::And, "&&"}, 
    {Operators::IsEqual, "=="}, 
    {Operators::IsNotEqual, "!="}, 
    {Operators::IsLess, "<"}, 
    {Operators::IsMore, ">"}, 
    {Operators::IsLessOrEqual, "<="}, 
    {Operators::IsMoreOrEqual, ">="}, 
    {Operators::Equate, "Equate"}, 
    {Operators::Unpack, "Unpack"}, 
    {Operators::Clone, "Clone"}, 
    {Operators::Index, "Index"}, 
    {Operators::ToTuple, "ToTuple"}, 
    {Operators::Length, "Length"}, 
    {Operators::Return, "Return"}, 
    {Operators::Jump, "Jump"}, 
    {Operators::Jz, "Jz"}, 
    {Operators::Jnz, "Jnz"}, 
    {Operators::Call, "Call"}, 
    {Operators::CallArg, "CallArg"}, 
    {Operators::UseInnerAccess, "Inner"}, 
    {Operators::UseMutableVars, "Mutable"}, 
    {Operators::Pop, "Pop"}, 
    {Operators::Print, "Print"},
    {Operators::PrintLf, "\\n"},
    {Operators::PrintMyName, "Me!"},
};   