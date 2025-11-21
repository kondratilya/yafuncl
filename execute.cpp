#include "execute.h"

Execute::Execute(SyntaxAnalys &syntax) {
    this->syntax = &syntax;
    this->syntax->Analyse();
    for (auto operation : this->syntax->code) {
        std::cout << std::string(*operation) << " ";
    }
    std::cout << std::endl;
}
Values::Value *Execute::Run() {
    return (new Context(*syntax))->Run();
}