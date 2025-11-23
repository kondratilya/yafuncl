#include <fstream>
#include "lexic.h"
#include "syntax.h"
#include "execute.h"

using namespace std;

int main(int argc, char*argv[]) {
    std::ifstream source;
    if (argc > 1) {
        source.open(argv[1]); 
    } else {
        source.open("test.txt"); 
    }

    LexicAnalys lexic(source);
    SyntaxAnalys syntax(lexic);
    Execute exec(syntax);
    exec.Run();
}
