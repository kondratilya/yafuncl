#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <stdexcept>
#include "syntax.h"
#include "context.h"
#include "values.h"

class Execute {
    SyntaxAnalys *syntax;
    std::ostream *output_;
    std::ostream *result_;

    public:
    Execute(SyntaxAnalys &syntax, std::ostream *result=NULL, std::ostream *output=NULL);
    Values::Value *Run();
};