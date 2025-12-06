#include "execute.h"

Execute::Execute(SyntaxAnalys &syntax, std::ostream *result, std::ostream *output) {
    this->syntax = &syntax;
    result_ = result;
    output_ = output;

    this->syntax->Analyse();
    
    if (output_) {
        int i = 0;
        for (auto instruction : this->syntax->code) {
            if (!(i%20))*output_ << i << ".  ";
            *output_ << std::string(*instruction) << " ";
            if (i%20 == 20-1) *output_ << std::endl;
            ++i;
        }
        *output_ << std::endl;
    }
}
Values::Value *Execute::Run() {
    return (new Context(*syntax, 0, result_, output_))->Run();
}