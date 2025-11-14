#include <iostream>
#include <fstream>
#include <format>
#include "lexic.h"
#include "syntax.h"
#include "execute.h"

using namespace std;

int main() {
    std::ifstream source("test.txt"); 

    LexicAnalys *lexic = new LexicAnalys(&source);
    SyntaxAnalys *syntax = new SyntaxAnalys(lexic);
    Execute *exec = new Execute(syntax);

    exec->Run();
}
