#pragma once
#include <set>
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

namespace RulesList {
    class Rule {
        public:
        Symbol left;
        std::vector<Symbol> right;
        Actions action;
    };
    const std::vector<Rule> rules = {
        {Symbol(NT::ROOT), {Symbol(NT::Program), Symbol(Tokens::END)}},

        {Symbol(NT::Program), {Symbol(NT::Expressions)}},
        {Symbol(NT::Expressions), {Symbol(NT::Expression), Symbol(";"), Symbol(NT::Expressions)}},
        {Symbol(NT::Expressions), {Symbol(NT::Expression)}},

        {Symbol(NT::Expression), {Symbol(NT::E)}, Actions::Pop},
        {Symbol(NT::Expression), {Symbol(Tokens::EMPTY)}},
        
        {Symbol(NT::BlockBegin), {Symbol("{")}, Actions::BeginFunction},
        {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol(NT::Expressions), Symbol("}")}, Actions::EndFunction},
        {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol("}")}, Actions::EndFunction},

        {Symbol(NT::FunctionDefinition), {Symbol(NT::Block)}},
        {Symbol(NT::FunctionCall), {Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCall},
        {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCallArg},

        {Symbol(NT::Tuple), {Symbol(NT::E), Symbol(","), Symbol(NT::E)}, Actions::TupleStart},
        {Symbol(NT::Tuple), {Symbol(NT::E), Symbol(","), NT::Tuple}, Actions::Tuple},
        
        {Symbol(NT::E), {Symbol(NT::T0)}},

        {Symbol(NT::T0), {Symbol(NT::T1)}},

        {Symbol(NT::T1), {Symbol(NT::T1), Symbol("+"), Symbol(NT::T2)}, Actions::Plus},
        {Symbol(NT::T1), {Symbol(NT::T1), Symbol("-"), Symbol(NT::T2)}, Actions::Minus},
        {Symbol(NT::T1), {Symbol(NT::T2)}},

        {Symbol(NT::T2), {Symbol(NT::T2), Symbol("*"), Symbol(NT::T3)}, Actions::Multiply},
        {Symbol(NT::T2), {Symbol(NT::T2), Symbol("/"), Symbol(NT::T3)}, Actions::Divide},
        {Symbol(NT::T2), {Symbol(NT::T3)}},

        {Symbol(NT::T3), {Symbol("-"), Symbol(NT::T2)}, Actions::Negative},
        {Symbol(NT::T3), {Symbol("+"), Symbol(NT::T2)}},
        {Symbol(NT::T3), {Symbol(NT::T4)}},
        {Symbol(NT::T3), {Symbol(":"), Symbol(NT::T3)}, Actions::Print},
        
        {Symbol(NT::T4), {Symbol(NT::V), Symbol("="), Symbol(NT::T0)}, Actions::Equate},
        {Symbol(NT::T4), {Symbol("("), Symbol(NT::E), Symbol(")")}},
        {Symbol(NT::T4), {Symbol(NT::FunctionDefinition)}},
        {Symbol(NT::T4), {Symbol(NT::FunctionCall)}},
        {Symbol(NT::T4), {Symbol(NT::V)}},
        
        {Symbol(NT::V), {Symbol(Tokens::ID)}, Actions::Name},
        {Symbol(NT::V), {Symbol("arguments")}},
        {Symbol(NT::V), {Symbol(NT::Tuple)}, Actions::TupleEnd},
        {Symbol(NT::V), {Symbol("("), Symbol(NT::Tuple), Symbol(")")}, Actions::TupleEnd},
        {Symbol(NT::V), {Symbol("("), Symbol(")")}, Actions::TupleEmpty},
        {Symbol(NT::V), {Symbol("("), Symbol(NT::E), Symbol(","), Symbol(")")}, Actions::TupleOne},


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
}
class Rules {
    size_t _size;
    public:
    Rules() { _size=RulesList::rules.size(); }
    const RulesList::Rule operator[] (size_t index) {
        return RulesList::rules[index];
    };
    const size() {
        return _size;
    };
    void GetTerminalsList(std::set<std::string> &keywords){
        for (auto r : RulesList::rules) {
            for (auto s : r.right) {
                if (s.IsTerminal())
                    keywords.insert(s.name);
            }
        }
    }
};