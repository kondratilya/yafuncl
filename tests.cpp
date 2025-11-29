#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

#include "tests.h"
#include "lexic.h"
#include "syntax.h"
#include "execute.h"

std::string numbers = "0={};1={++0;};2={++1;};3={++2;};4={++3;};5={++4;};6={++5;};7={++6;};8={++7;};9={++8;};10={++9;};";

void test(std::string test) {
    std::istringstream source(test);
    std::ofstream null_stream;
    LexicAnalys lexic(source);
    SyntaxAnalys syntax(lexic, &null_stream);
    Execute exec(syntax);
    exec.Run();
}

void tests() {
    test(numbers);
}

