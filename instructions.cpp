#include "instructions.h"


ReturnCode OperatorInstruction::Run(Context *context) {
    switch (op) {
        case Operators::Jump:
            context->Jump(context->Pop()->GetAddress());
            return ReturnCode::Continue;
        case Operators::Call:
            context->Push((new Context(context, context->Pop()->GetAddress()))->Run());
        break;
        case Operators::CallArg: {
            Address address = context->Pop()->GetAddress();
            Value *arguments = new Value(context->Pop());
            context->Push((new Context(context, address, arguments))->Run());
        } break;
        case Operators::Return:
            return ReturnCode::Return;
        case Operators::Pop:
            context->Result(new Value(context->Pop()));
        break;
        case Operators::Add: 
            context->Push(new Value(context->Pop()->GetValue(context)+context->Pop()->GetValue(context)));
        break;
        case Operators::Negative:
            context->Push(new Value(-context->Pop()->GetValue(context)));
        break;
        case Operators::Positive:
            context->Push(new Value(context->Pop()->GetValue(context)));
        break;
        case Operators::Inc: 
            context->Push(new Value(context->Pop()->GetValue(context)+1));      //Неявное объявление 1
        break;
        case Operators::Dec: 
            context->Push(new Value(context->Pop()->GetValue(context)-1));
        break;
        case Operators::Not:
            context->Push(new Value(static_cast<DefaultValueType>(!context->Pop()->GetValue(context))));
         break;

        case Operators::Equate: {
            Value *v = context->Pop();
            context->Top()->Define(v);
        } break;
        case Operators::Print: {
            std::cout << "\"" << context->Top() << "\" ";
        } break;                        
    }
    return ReturnCode::None;
}

ReturnCode VariableInstruction::Run(Context *context) {
    /*if (names_lookup_table) {
        context->Push(context->GetVariable(index, names_lookup_table->get(index)));
    } else {
        context->Push(context->GetVariable(index));
    }*/
   context->Push(context->GetVariable(index));
    return ReturnCode::None;
}

ReturnCode AddressInstruction::Run(Context *context) {
    context->Push(new Value(static_cast<Address>(position)));
    return ReturnCode::None;
}

ReturnCode TupleInstruction::Run(Context *context) {
    Tuple *t = new Tuple;
    for (int i=0;i<size;++i) {
        t->push_front(context->Pop());
    }
    Value*v = new Value(t);
    context->Push(v);
    return ReturnCode::None;
}
