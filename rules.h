#pragma once
#include <set>
#include <algorithm>
#include "symbols.h"
// #include "instructions.h"

using namespace Symbols;

using NT = NonTerminals;

enum class Actions {
    None=0, Name, Equate, Print,
    BeginFunction, EndFunction, Return, ReturnEmpty,
    Plus, Minus, Multiply, Divide, 
    Negative, Positive, Dec, Inc, Not,
    Pop,
    Tuple, TupleEmpty, TupleOne, TupleStart, TupleEnd, EquateTuple,
    FunctionDefinition, 
    FunctionCall, FunctionCallArg,
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
    const std::vector<Rule> rules = {
        {NT::ROOT, {Symbol(NT::Program), Symbol(Tokens::END)}},

        {NT::Program, {Symbol(NT::Expressions)}},
        {NT::Expressions, {Symbol(NT::Expression), Symbol(";"), Symbol(NT::Expressions)}},
        {NT::Expressions, {Symbol(NT::Expression)}},

        {NT::Expression, {Symbol(NT::E)}, Actions::Pop},
        {NT::Expression, {Symbol("return"), Symbol(NT::E)}, Actions::Return},
        {NT::Expression, {Symbol("return"), Symbol(";")}, Actions::ReturnEmpty},  // ReturnEmpty: Messing with stack to mare empty return work;
        {NT::Expression, {Symbol(Tokens::EMPTY)}},
        
        {NT::BlockBegin, {Symbol("{")}, Actions::BeginFunction},
        {NT::BlockBegin, {Symbol(NT::Modifyers), Symbol(NT::BlockBegin)}},
        {NT::Block, {Symbol(NT::BlockBegin), Symbol(NT::Expressions), Symbol("}")}, Actions::EndFunction},
        {NT::Block, {Symbol(NT::BlockBegin), Symbol("}")}, Actions::EndFunction},

        {NT::E, {Symbol(NT::T0)}},

        {NT::T0, {Symbol(NT::T1)}},

        {NT::Tuple, {Symbol(NT::T1), Symbol(","), Symbol(NT::T1)}, Actions::TupleStart},
        {NT::Tuple, {Symbol(NT::T1), Symbol(","), NT::Tuple}, Actions::Tuple},

        {NT::T1, {Symbol(NT::T2)}},

        {NT::T2, {Symbol(NT::T2), Symbol("or"), Symbol(NT::T3)}, Actions::Plus},
        {NT::T2, {Symbol(NT::T3)}},

        {NT::T3, {Symbol(NT::T3), Symbol("and"), Symbol(NT::T4)}, Actions::Plus},
        {NT::T3, {Symbol(NT::T4)}},

        {NT::T4, {Symbol(NT::T4), Symbol("=="), Symbol(NT::T5)}, Actions::Plus},
        {NT::T4, {Symbol(NT::T4), Symbol("!="), Symbol(NT::T5)}, Actions::Plus},
        {NT::T4, {Symbol(NT::T4), Symbol("equals"), Symbol(NT::T5)}, Actions::Plus},
        {NT::T4, {Symbol(NT::T4), Symbol("not"), Symbol("equals"), Symbol(NT::T5)}, Actions::Plus},
        {NT::T4, {Symbol(NT::T5)}},

        {NT::T5, {Symbol(NT::T5), Symbol("<"), Symbol(NT::T6)}, Actions::Plus},
        {NT::T5, {Symbol(NT::T5), Symbol(">"), Symbol(NT::T6)}, Actions::Plus},
        {NT::T5, {Symbol(NT::T5), Symbol("<="), Symbol(NT::T6)}, Actions::Plus},
        {NT::T5, {Symbol(NT::T5), Symbol(">="), Symbol(NT::T6)}, Actions::Plus},
        {NT::T5, {Symbol(NT::T6)}},

        {NT::T6, {Symbol(NT::T6), Symbol("+"), Symbol(NT::T7)}, Actions::Plus},
        {NT::T6, {Symbol(NT::T6), Symbol("-"), Symbol(NT::T7)}, Actions::Minus},
        {NT::T6, {Symbol(NT::T7)}},

        {NT::T7, {Symbol(NT::T7), Symbol("*"), Symbol(NT::T8)}, Actions::Multiply},
        {NT::T7, {Symbol(NT::T7), Symbol("/"), Symbol(NT::T8)}, Actions::Divide},
        {NT::T7, {Symbol(NT::T7), Symbol("%"), Symbol(NT::T8)}, Actions::Divide},
        {NT::T7, {Symbol(NT::T8)}},

        {NT::T8, {Symbol("-"), Symbol(NT::T8)}, Actions::Negative},
        {NT::T8, {Symbol("+"), Symbol(NT::T8)}},
        {NT::T8, {Symbol("--"), Symbol(NT::T8)}, Actions::Dec},
        {NT::T8, {Symbol("++"), Symbol(NT::T8)}, Actions::Inc},
        {NT::T8, {Symbol("!"), Symbol(NT::T8)}, Actions::Not},
        {NT::T8, {Symbol("not"), Symbol(NT::T8)}, Actions::Not},
        {NT::T8, {Symbol("@"), Symbol(NT::T8)}, Actions::Print},
        {NT::T8, {Symbol(NT::T9)}},

        {NT::FunctionCall, {Symbol("=>"), Symbol(NT::Block)}, Actions::FunctionCall},
        {NT::FunctionCall, {Symbol("=>"), Symbol(NT::NAME)}, Actions::FunctionCall},
        {NT::FunctionCall, {Symbol(NT::E), Symbol("=>"), Symbol(NT::Block)}, Actions::FunctionCallArg},
        {NT::FunctionCall, {Symbol(NT::E), Symbol("=>"), Symbol(NT::NAME)}, Actions::FunctionCallArg},

        {NT::T9, {Symbol(NT::FunctionCall)}},
        {NT::T9, {Symbol(NT::LVALUE), Symbol("="), Symbol(NT::E)}, Actions::Equate},
        {NT::T9, {Symbol(NT::Tuple), Symbol("="), Symbol(NT::E)}, Actions::EquateTuple},
        {NT::T9, {Symbol(NT::Tuple)}, Actions::TupleEnd},
        {NT::T9, {Symbol("("), Symbol(NT::E), Symbol(")")}},
        {NT::T9, {Symbol(NT::Block)}},
        {NT::T9, {Symbol(NT::LVALUE)}},
        
        
        {NT::Tuple, {Symbol("("), Symbol(NT::Tuple), Symbol(")")}},
        // {NT::Tuple, {Symbol(NT::Tuple)}, Actions::TupleEnd},
        // {NT::LVALUE, {Symbol("("), Symbol(NT::Tuple), Symbol(","), Symbol(")")}, Actions::TupleEnd},
        {NT::Tuple, {Symbol("("), Symbol(")")}, Actions::TupleEmpty},
        {NT::Tuple, {Symbol("("), Symbol(NT::T1), Symbol(","), Symbol(")")}, Actions::TupleOne},
        {NT::LVALUE, {Symbol(NT::NAME)}},

        {NT::NAME, {Symbol(Tokens::ID)}, Actions::Name},
        {NT::NAME, {Symbol(NT::Modifyers), Symbol(NT::NAME)}},

        {NT::Modifyers, {Symbol(NT::Modifyer), Symbol(NT::Modifyers)}},
        {NT::Modifyers, {Symbol(NT::Modifyer)}},

        {NT::Modifyer, {Symbol("local")}},
        {NT::Modifyer, {Symbol("global")}},
    };

    public:
    Rules() { _size=rules.size(); }
    const Rule operator[] (size_t index) {
        return rules[index];
    };
    const int size() {
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



// :((1,2)=(3,4));
// a=(1,2,3+2,4);
// :a;
// print={:arguments;};


// (a,2)=>print;
// =>print;
// 1=>print;

// #0=(1,2,3>{a=3;:a;});
// #0=(1,2,3>{:a;a=4;:a;});
// #:a;
// #:0;
// #3+a;
// #:((a,b)=(c,d));
// #:a;
// #:b;
// #5={};
// #:tuple=(0,(1, {}, :(), 3+a, :4), 5, 6, (7,), :8);


// #:b,
// #(12,13,3)
// #a=b=c=0;
// #:a;:b;:c;
// #:x;
// #x=(x+a);
// #(1,) * 3;

// #{:y};
// #:x;
// #x=x+x;
// #:x;
// #a={1+1};
// #:a

// #h={f; v; k};jj;{}


// #3 = {inc(2);};
// #add = a, b>{a+b;};
// #print(add(1, 3));
// #print({++3});
// #print((a,b){a+b}(1,3));


// #    a  o  s  / \n
// #S   A  O  S C1  S
// #A   A  S  S  S  S
// #O1  S O2  S O2  S
// #O2  S  S  S  S  S
// #C1  S O2  S C2  S
// #C2 C2 C2 C2 C2  S
// #tuples

// # Program -> FunctionCall '$'
// # FunctionCall -> '>' Name | '>' Block | '>' Name Tuple | '>' Block Tuple
// # Block -> BlockBegin Expressions '}' | BlockBegin '}'
// # BlockBegin -> Tuple '{' | '{'
// # Expressions -> Expression Expressions  | Expression
// # Tuple -> Expression, Tuple | Expression
// # Expression -> V