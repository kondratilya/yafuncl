#include <fstream>
#include "lexic.h"
#include "syntax.h"
#include "execute.h"

using namespace std;

int main(int argc, char*argv[]) {
    std::ifstream source("test.txt"); 

    LexicAnalys lexic(source);
    SyntaxAnalys syntax(lexic);
    Execute exec(syntax);
    exec.Run();
}
