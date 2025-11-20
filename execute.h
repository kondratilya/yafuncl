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

    public:
    Execute(SyntaxAnalys &syntax);
    Value *Run();
};