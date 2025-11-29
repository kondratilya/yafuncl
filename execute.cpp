#include "execute.h"

Execute::Execute(SyntaxAnalys &syntax, std::ostream *result, std::ostream *output) {
    this->syntax = &syntax;
    result_ = result;
    output_ = output;

    this->syntax->Analyse();
    
    if (output_) {
        for (auto instruction : this->syntax->code) {
            *output_ << std::string(*instruction) << " ";
        }
        *output_ << std::endl;
    }
}
Values::Value *Execute::Run() {
    return (new Context(*syntax, 0, result_, output_))->Run();
}