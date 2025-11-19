#pragma once
#include <set>
#include <algorithm>
#include "symbols.h"

using namespace Symbols;

using NT = NonTerminals;

enum class Actions {
    None=0, Name, Equate, Print,
    BeginFunction, EndFunction, Return, Plus, Minus, Multiply, Divide, Negative, 
    Pop,
    Tuple, TupleEmpty, TupleOne, TupleStart, TupleEnd,
    FunctionDefinition, 
    FunctionCall, FunctionCallArg,
};


class Rule {
    public:
    Symbol left;
    std::vector<Symbol> right;
    Actions action;
};

class Rules {
    size_t _size;
    size_t max_right_size=0;
    const std::vector<Rule> rules = {
        {Symbol(NT::ROOT), {Symbol(NT::Program), Symbol(Tokens::END)}},

        {Symbol(NT::Program), {Symbol(NT::Expressions)}},
        {Symbol(NT::Expressions), {Symbol(NT::Expression), Symbol(";"), Symbol(NT::Expressions)}},
        {Symbol(NT::Expressions), {Symbol(NT::Expression)}},

        {Symbol(NT::Expression), {Symbol(NT::E)}, Actions::Pop},
        {Symbol(NT::Expression), {Symbol(Tokens::EMPTY)}},
        
        
        //{Symbol(NT::BlockBegin), {Symbol(NT::Modifyers), Symbol("{")}, Actions::BeginFunction},
        {Symbol(NT::BlockBegin), {Symbol("{")}, Actions::BeginFunction},
        {Symbol(NT::BlockBegin), {Symbol(NT::Modifyers), Symbol(NT::BlockBegin)}},
        {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol(NT::Expressions), Symbol("}")}, Actions::EndFunction},
        {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol("}")}, Actions::EndFunction},

        //>> {Symbol(NT::FunctionDefinition), {Symbol(NT::Block)}},
        // {Symbol(NT::FunctionCall), {Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCall},
        // {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCallArg},

        // {Symbol(NT::Tuple), {Symbol(NT::E), Symbol(","), Symbol(NT::E)}, Actions::TupleStart},
        // {Symbol(NT::Tuple), {Symbol(NT::E), Symbol(","), NT::Tuple}, Actions::Tuple},
        
        {Symbol(NT::E), {Symbol(NT::T0)}},

        // {Symbol(NT::T0), {Symbol(NT::V), Symbol("="), Symbol(NT::E)}, Actions::Equate},
        {Symbol(NT::T0), {Symbol(NT::T1)}},

        {Symbol(NT::Tuple), {Symbol(NT::T1), Symbol(","), Symbol(NT::T1)}, Actions::TupleStart},
        {Symbol(NT::Tuple), {Symbol(NT::T1), Symbol(","), NT::Tuple}, Actions::Tuple},

        {Symbol(NT::T1), {Symbol(NT::T2)}},

        {Symbol(NT::T2), {Symbol(NT::T2), Symbol("or"), Symbol(NT::T3)}, Actions::Plus},
        {Symbol(NT::T2), {Symbol(NT::T3)}},

        {Symbol(NT::T3), {Symbol(NT::T3), Symbol("and"), Symbol(NT::T4)}, Actions::Plus},
        {Symbol(NT::T3), {Symbol(NT::T4)}},

        {Symbol(NT::T4), {Symbol(NT::T4), Symbol("=="), Symbol(NT::T5)}, Actions::Plus},
        {Symbol(NT::T4), {Symbol(NT::T4), Symbol("!="), Symbol(NT::T5)}, Actions::Plus},
        {Symbol(NT::T4), {Symbol(NT::T4), Symbol("equals"), Symbol(NT::T5)}, Actions::Plus},
        {Symbol(NT::T4), {Symbol(NT::T4), Symbol("not"), Symbol("equals"), Symbol(NT::T5)}, Actions::Plus},
        {Symbol(NT::T4), {Symbol(NT::T5)}},

        {Symbol(NT::T5), {Symbol(NT::T5), Symbol("<"), Symbol(NT::T6)}, Actions::Plus},
        {Symbol(NT::T5), {Symbol(NT::T5), Symbol(">"), Symbol(NT::T6)}, Actions::Plus},
        {Symbol(NT::T5), {Symbol(NT::T5), Symbol("<="), Symbol(NT::T6)}, Actions::Plus},
        {Symbol(NT::T5), {Symbol(NT::T5), Symbol(">="), Symbol(NT::T6)}, Actions::Plus},
        {Symbol(NT::T5), {Symbol(NT::T6)}},

/*29*/  {Symbol(NT::T6), {Symbol(NT::T6), Symbol("+"), Symbol(NT::T7)}, Actions::Plus},
        {Symbol(NT::T6), {Symbol(NT::T6), Symbol("-"), Symbol(NT::T7)}, Actions::Minus},
        {Symbol(NT::T6), {Symbol(NT::T7)}},

/*32*/  {Symbol(NT::T7), {Symbol(NT::T7), Symbol("*"), Symbol(NT::T8)}, Actions::Multiply},
        {Symbol(NT::T7), {Symbol(NT::T7), Symbol("/"), Symbol(NT::T8)}, Actions::Divide},
        {Symbol(NT::T7), {Symbol(NT::T7), Symbol("%"), Symbol(NT::T8)}, Actions::Divide},
        {Symbol(NT::T7), {Symbol(NT::T8)}},

/*35*/  {Symbol(NT::T8), {Symbol("-"), Symbol(NT::T8)}, Actions::Negative},
        {Symbol(NT::T8), {Symbol("+"), Symbol(NT::T8)}},
        {Symbol(NT::T8), {Symbol("--"), Symbol(NT::T8)}, Actions::Negative},
        {Symbol(NT::T8), {Symbol("++"), Symbol(NT::T8)}},
        {Symbol(NT::T8), {Symbol("!"), Symbol(NT::T8)}, Actions::Negative},
        {Symbol(NT::T8), {Symbol("not"), Symbol(NT::T8)}},
        {Symbol(NT::T8), {Symbol("@"), Symbol(NT::T8)}, Actions::Print},
        {Symbol(NT::T8), {Symbol(NT::T9)}},
        
        {Symbol(NT::FunctionCall), {Symbol("=>"), Symbol(NT::Block)}, Actions::FunctionCall},
        {Symbol(NT::FunctionCall), {Symbol("=>"), Symbol(NT::NAME)}, Actions::FunctionCall},
        {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol("=>"), Symbol(NT::Block)}, Actions::FunctionCallArg},
        {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol("=>"), Symbol(NT::NAME)}, Actions::FunctionCallArg},

        {Symbol(NT::T9), {Symbol(NT::FunctionCall)}},
        {Symbol(NT::T9), {Symbol(NT::LVALUE), Symbol("="), Symbol(NT::E)}, Actions::Equate},
        {Symbol(NT::T9), {Symbol("("), Symbol(NT::E), Symbol(")")}},
        {Symbol(NT::T9), {Symbol(NT::Block)}},
        {Symbol(NT::T9), {Symbol(NT::LVALUE)}},
        
        {Symbol(NT::LVALUE), {Symbol(NT::Tuple)}, Actions::TupleEnd},
        {Symbol(NT::LVALUE), {Symbol("("), Symbol(NT::Tuple), Symbol(","), Symbol(")")}, Actions::TupleEnd},
        {Symbol(NT::LVALUE), {Symbol("("), Symbol(")")}, Actions::TupleEmpty},
        {Symbol(NT::LVALUE), {Symbol("("), Symbol(NT::T1), Symbol(","), Symbol(")")}, Actions::TupleOne},
        {Symbol(NT::LVALUE), {Symbol(NT::NAME)}},

        {Symbol(NT::NAME), {Symbol(Tokens::ID)}, Actions::Name},
        {Symbol(NT::NAME), {Symbol(NT::Modifyers), Symbol(NT::NAME)}},

        {Symbol(NT::Modifyers), {Symbol(NT::Modifyer), Symbol(NT::Modifyers)}},
        {Symbol(NT::Modifyers), {Symbol(NT::Modifyer)}},

        {Symbol(NT::Modifyer), {Symbol("local")}},
        {Symbol(NT::Modifyer), {Symbol("global")}},


    //    E   :-  V = E     |   T1
    //    T1  :-  T2 + T1   |   T2 - T1  |  T2
    //    T2  :-  T3 * T2   |   T3 / T2  |  T3
    //    T3  :- -T4        |  +T4       |  T4
    //    T4  :-  NAME(LST) |   T5
    //    T5  :-  (E)       |   N        |  V
    //    LST :-   E,LST    |   E
    //    V   :-   NAME
    /*
    S=E;
    E=E'+'T|E'-'T;
    E=T;
    T= T'*'F| T'/'F|F;

    F='('E')';
    F=per;
    F='-'T;
    */
        //{Symbol(NT::Expressions), {Symbol(NT::Expression)}, Symbol(";"), Symbol(NT::Expressions)}},
        
        // {Symbol(NT::S), {Symbol(NT::E), Symbol(END)}},

        // {Symbol(NT::E), {Symbol(NT::E), Symbol("+"), Symbol(NT::T)}},
        // {Symbol(NT::E), {Symbol(NT::E), Symbol("-"), Symbol(NT::T)}},
        // {Symbol(NT::E), {Symbol(NT::T)}},

        // {Symbol(NT::T), {Symbol(NT::T), Symbol("*"), Symbol(NT::F)}},
        // {Symbol(NT::T), {Symbol(NT::T), Symbol("/"), Symbol(NT::F)}},
        // {Symbol(NT::T), {Symbol(NT::F)}},

        // {Symbol(NT::F), {Symbol("("), Symbol(NT::E), Symbol(")")}},
        // {Symbol(NT::F), {Symbol("-"), Symbol(NT::T)}},
        // {Symbol(NT::F), {Symbol(ID)}},
    };

//     const std::vector<Rule> rules1 = {
//         {Symbol(NT::ROOT), {Symbol(NT::Program), Symbol(Tokens::END)}},

//         {Symbol(NT::Program), {Symbol(NT::Expressions)}},
//         {Symbol(NT::Expressions), {Symbol(NT::Expression), Symbol(";"), Symbol(NT::Expressions)}},
//         {Symbol(NT::Expressions), {Symbol(NT::Expression)}},

//         {Symbol(NT::Expression), {Symbol(NT::E)}, Actions::Pop},
//         {Symbol(NT::Expression), {Symbol(Tokens::EMPTY)}},
        
//         {Symbol(NT::BlockBegin), {Symbol("{")}, Actions::BeginFunction},
//         {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol(NT::Expressions), Symbol("}")}, Actions::EndFunction},
//         {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol("}")}, Actions::EndFunction},

//         {Symbol(NT::FunctionDefinition), {Symbol(NT::Block)}},
//         {Symbol(NT::FunctionCall), {Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCall},
//         {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCallArg},

//         {Symbol(NT::Tuple), {Symbol(NT::E), Symbol(","), Symbol(NT::E)}, Actions::TupleStart},
//         {Symbol(NT::Tuple), {Symbol(NT::E), Symbol(","), NT::Tuple}, Actions::Tuple},
        
//         {Symbol(NT::E), {Symbol(NT::T0)}},

// /*15*/  {Symbol(NT::T0), {Symbol(NT::T1)}},

//         {Symbol(NT::T1), {Symbol(NT::T1), Symbol("+"), Symbol(NT::T2)}, Actions::Plus},
//         {Symbol(NT::T1), {Symbol(NT::T1), Symbol("-"), Symbol(NT::T2)}, Actions::Minus},
// /*18*/  {Symbol(NT::T1), {Symbol(NT::T2)}},

//         {Symbol(NT::T2), {Symbol(NT::T2), Symbol("*"), Symbol(NT::T3)}, Actions::Multiply},
//         {Symbol(NT::T2), {Symbol(NT::T2), Symbol("/"), Symbol(NT::T3)}, Actions::Divide},
// /*21*/  {Symbol(NT::T2), {Symbol(NT::T3)}},

// /*22*/  {Symbol(NT::T3), {Symbol("-"), Symbol(NT::T3)}, Actions::Negative}, /// ->T2
//         {Symbol(NT::T3), {Symbol("+"), Symbol(NT::T3)}},
//         {Symbol(NT::T3), {Symbol(":"), Symbol(NT::T3)}, Actions::Print}, /// -> T2
//         {Symbol(NT::T3), {Symbol(NT::T4)}},
        
//         {Symbol(NT::T4), {Symbol(NT::V), Symbol("="), Symbol(NT::T0)}, Actions::Equate},
//         {Symbol(NT::T4), {Symbol("("), Symbol(NT::E), Symbol(")")}},
//         {Symbol(NT::T4), {Symbol(NT::FunctionDefinition)}},
//         {Symbol(NT::T4), {Symbol(NT::FunctionCall)}},
// /*30*/  {Symbol(NT::T4), {Symbol(NT::V)}},
        
// /*31*/  {Symbol(NT::V), {Symbol(Tokens::ID)}, Actions::Name},
//         {Symbol(NT::V), {Symbol("arguments")}},
//         {Symbol(NT::V), {Symbol(NT::Tuple)}, Actions::TupleEnd},
//         {Symbol(NT::V), {Symbol("("), Symbol(NT::Tuple), Symbol(")")}, Actions::TupleEnd},
//         {Symbol(NT::V), {Symbol("("), Symbol(")")}, Actions::TupleEmpty},
//         {Symbol(NT::V), {Symbol("("), Symbol(NT::E), Symbol(","), Symbol(")")}, Actions::TupleOne},
//     };


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