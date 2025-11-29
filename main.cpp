#include <fstream>
#include "lexic.h"
#include "syntax.h"
#include "execute.h"
#ifdef TESTS
#include "tests.h"
#endif

using namespace std;

int main(int argc, char*argv[]) {
    std::ifstream source;
    if (argc > 1) {
        source.open(argv[1]); 
    } else {
        source.open("test.txt"); 
    }

#ifndef TESTS
    std::ofstream null_stream;
    LexicAnalys lexic(source);
    SyntaxAnalys syntax(lexic, NULL);
    Execute exec(syntax, &cout, &cout);
    exec.Run();
#else
    tests();
#endif

}
