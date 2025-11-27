#pragma once
#include <set>
#include <algorithm>
#include "symbols.h"

using namespace Symbols;
using namespace std::string_literals;

using NT = NonTerminals;

enum class Actions {
    None=0, Name, 
    ModifyerInner, ModifyerOuter, ModifyerMutable, ModifyerImmutable, 
    Equate, 
    Print, PrintLn, PrintChar, PrintMyName,
    BeginFunction, EndFunction, 
    Return, ReturnEmpty, ReturnIf, ReturnEmptyIf, ReturnUnconditional,
    Plus, Minus, Multiply, Divide, Mod,
    Negative, Positive, Dec, Inc, Not,
    Or, And, IsEqual, IsNotEqual, IsLess, IsMore, IsLessOrEqual, IsMoreOrEqual, 
    Pop,
    Tuple, TupleEmpty, TupleOne, TupleEnd, Unpack, CloneTuple,
    Index, ToTuple, Length,
    FunctionCall, FunctionCallArg,
    If, Then, ThenEnd, Else,
    While, WhileDo, WhileEnd,

};


class Rule {
    public:
    NonTerminals left;
    std::vector<Symbol> right;
    Actions action;
};

class Rules {
    size_t _size;
    size_t max_right_size=0;

    const static std::vector<Rule> rules;

    public:
    Rules() { _size=rules.size(); }
    const Rule operator[] (size_t index) {
        return rules[index];
    };
    const int size() const {
        return _size;
    };
    const size_t MaxRuleSize() {
        if (max_right_size) return max_right_size;
        for (auto r : rules) {
            max_right_size = std::max(max_right_size, r.right.size());
        }
        return max_right_size;
    };
    void GetTerminalsList(std::set<std::string> &keywords){
        for (auto r : rules) {
            for (auto s : r.right) {
                if (s.IsTerminal())
                    keywords.insert(s.name);
            }
        }
    }
};
