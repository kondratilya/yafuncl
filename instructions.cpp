#include "instructions.h"

ReturnCode OperatorInstruction::Run(Context *context) {
    switch (op) {
        case Operators::Jump:
            context->Jump(context->Pop()->GetAddress().position);
            return ReturnCode::Continue;
        case Operators::Call: {
            Values::Address address = context->Pop()->GetAddress();
            Context *new_context = address.getContext(context);
            context->Push(address.getContext(context)->Run());
        } break;
        case Operators::CallArg: {
            Values::Address address = context->Pop()->GetAddress();
            Values::Value *arguments = new Values::Value(context->Pop());
            context->Push(address.getContext(context)->WithArgument(arguments)->Run());
            // Address address = context->Pop()->GetAddress();
            // Values::Value *arguments = new Values::Value(context->Pop());
            // context->Push((new Context(context, address, arguments))->Run());
        } break;
        case Operators::Return:
            return ReturnCode::Return;
        case Operators::Pop:
            context->Result(new Values::Value(context->Pop()));
        break;
        case Operators::Add: 
            context->Push(new Values::Value(context->Pop()->GetValue(context)+context->Pop()->GetValue(context)));
        break;
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

        case Operators::Equate: {
            Values::Value *v = context->Pop();
            context->Top()->Define(v);
        } break;
        case Operators::Print: {
            std::cout << "\"" << std::string(*context->Top()) << "\" ";
        } break;                        
    }
    return ReturnCode::None;
}

ReturnCode VariableInstruction::Run(Context *context) {
   context->Push(context->GetVariable(index));
    return ReturnCode::None;
}

ReturnCode AddressInstruction::Run(Context *context) {
    // context->Push(new Values::Value(static_cast<size_t>(position)));
    context->Push(new Values::Value(Values::Address(position)));
    return ReturnCode::None;
}

ReturnCode TupleInstruction::Run(Context *context) {
    Values::Tuple *t = new Values::Tuple;
    for (int i=0;i<size;++i) {
        t->push_front(context->Pop());
    }
    Values::Value*v = new Values::Value(t);
    context->Push(v);
    return ReturnCode::None;
}
