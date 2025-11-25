#pragma once
#include <set>
#include <algorithm>
#include "symbols.h"

using namespace Symbols;
using namespace std::string_literals;

using NT = NonTerminals;

enum class Actions {
    None=0, Name, 
    ModifyerInner, ModifyerOuter,
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
    const std::vector<Rule> rules = {
        {NT::ROOT, {NT::Program, Tokens::END}},

        {NT::Program, {NT::Expressions}},
        {NT::Expressions, {NT::Expression, ";"s, NT::Expressions}},
        {NT::Expressions, {NT::Expression}},

        {NT::Expression, {NT::E}, Actions::Pop},
        {NT::Expression, {NT::Return}, Actions::ReturnUnconditional},
        {NT::Expression, {NT::Return, "if"s, NT::E}, Actions::ReturnIf},
        {NT::Expression, {NT::ReturnEmpty}, Actions::ReturnUnconditional},
        {NT::Expression, {NT::ReturnEmpty, "if"s, NT::E}, Actions::ReturnEmptyIf},
        {NT::Expression, {Symbol(Tokens::EMPTY)}},

        {NT::Return, {"return"s, NT::E}, Actions::Return},
        {NT::ReturnEmpty, {"$$$returnempty$$$"s}, Actions::ReturnEmpty},  // ReturnEmpty: Messing with stack to mare empty return work;
        
        {NT::BlockBegin, {"{"s}, Actions::BeginFunction},
        {NT::BlockBegin, {NT::Modifyers, NT::BlockBegin}},
        {NT::Block, {NT::BlockBegin, NT::Expressions, "}"s}, Actions::EndFunction},
        {NT::Block, {NT::BlockBegin, "}"s}, Actions::EndFunction},

        {NT::E, {NT::T0}},

        {NT::T0, {NT::T1}},

        {NT::List, {NT::T1, ","s, NT::T1}, Actions::Tuple},
        {NT::List, {NT::T1, ","s, NT::List}, Actions::Tuple},

        {NT::T1, {NT::T2}},

        {NT::T2, {NT::T2, "or"s, NT::T3}, Actions::Or},
        {NT::T2, {NT::T2, "||"s, NT::T3}, Actions::Or},
        {NT::T2, {NT::T3}},

        {NT::T3, {NT::T3, "and"s, NT::T4}, Actions::And},
        {NT::T3, {NT::T3, "&&"s, NT::T4}, Actions::And},
        {NT::T3, {NT::T4}},

        {NT::T4, {NT::T4, "=="s, NT::T5}, Actions::IsEqual},
        {NT::T4, {NT::T4, "!="s, NT::T5}, Actions::IsNotEqual},
        {NT::T4, {NT::T4, "equals"s, NT::T5}, Actions::IsEqual},
        {NT::T4, {NT::T4, "notequals"s, NT::T5}, Actions::IsNotEqual},
        {NT::T4, {NT::T5}},

        {NT::T5, {NT::T5, "<"s, NT::T6}, Actions::IsLess},
        {NT::T5, {NT::T5, ">"s, NT::T6}, Actions::IsMore},
        {NT::T5, {NT::T5, "<="s, NT::T6}, Actions::IsLessOrEqual},
        {NT::T5, {NT::T5, ">="s, NT::T6}, Actions::IsMoreOrEqual},
        {NT::T5, {NT::T6}},

        {NT::T6, {NT::T6, "+"s, NT::T7}, Actions::Plus},
        {NT::T6, {NT::T6, "-"s, NT::T7}, Actions::Minus},
        {NT::T6, {NT::T7}},

        {NT::T7, {NT::T7, "*"s, NT::T8}, Actions::Multiply},
        {NT::T7, {NT::T7, "/"s, NT::T8}, Actions::Divide},
        {NT::T7, {NT::T7, "%"s, NT::T8}, Actions::Mod},
        {NT::T7, {NT::T8}},

        {NT::T8, {"-"s, NT::T8}, Actions::Negative},
        {NT::T8, {"+"s, NT::T8}, Actions::Positive},
        {NT::T8, {"--"s, NT::T8}, Actions::Dec},
        {NT::T8, {"++"s, NT::T8}, Actions::Inc},
        {NT::T8, {"!"s, NT::T8}, Actions::Not},
        {NT::T8, {"not"s, NT::T8}, Actions::Not},
        {NT::T8, {"..."s, NT::T8}, Actions::ToTuple},
        {NT::T8, {"@"s, NT::T8}, Actions::Print},
        {NT::T8, {"@@"s, NT::T8}, Actions::PrintLn},
        {NT::T8, {"@@@"s, NT::T8}, Actions::PrintChar},
        {NT::T8, {"@?"s, NT::T8}, Actions::PrintMyName},
        {NT::T8, {NT::T9}},

        {NT::FunctionCall, {"=>"s, NT::Block}, Actions::FunctionCall},
        {NT::FunctionCall, {"=>"s, NT::NAME}, Actions::FunctionCall},
        {NT::FunctionCall, {NT::T9, "=>"s, NT::Block}, Actions::FunctionCallArg},
        {NT::FunctionCall, {NT::T9, "=>"s, NT::NAME}, Actions::FunctionCallArg},
        {NT::FunctionCall, {NT::T9, "=>"s, "length"s}, Actions::Length},
       
        {NT::T9, {NT::FunctionCall}},
        {NT::T9, {NT::LVALUE, "="s, NT::E}, Actions::Equate},
        {NT::T9, {NT::Tuple, "="s, NT::E}, Actions::Unpack},
        {NT::T9, {NT::Tuple}, Actions::CloneTuple},         // Отвязать котреж от переменных
        {NT::T9, {"("s, NT::E, ")"s}},
        {NT::T9, {NT::Block}},
        {NT::T9, {NT::LVALUE}},
        {NT::T9, {NT::IfThen}, Actions::ThenEnd},
        {NT::T9, {NT::IfElse}},
        {NT::T9, {NT::WhileEnd}},
        {NT::T9, {NT::WhileDo}, Actions::WhileEnd},
        
        {NT::If, {"if"s, NT::E}, Actions::If},
        {NT::IfThen, {NT::If, ":"s, NT::E}, Actions::Then},
        {NT::IfElse, {NT::IfThen, "else"s, ":"s, NT::E}, Actions::Else},

        {NT::While, {"while"s}, Actions::While},
        {NT::WhileDo, {NT::While, NT::E}, Actions::WhileDo},
        {NT::WhileEnd, {NT::WhileDo, ":"s, NT::E}, Actions::WhileEnd},
        
        {NT::Tuple, {"("s, NT::List, ")"s}, Actions::TupleEnd},
        {NT::Tuple, {"("s, NT::T1, ","s, ")"s}, Actions::TupleOne},
        {NT::Tuple, {"("s, ")"s}, Actions::TupleEmpty},

        {NT::LVALUE, {NT::T9, "["s, NT::E, "]"s}, Actions::Index}, 
        {NT::LVALUE, {NT::NAME}},

        {NT::NAME, {Symbol(Tokens::ID)}, Actions::Name},
        {NT::NAME, {NT::Modifyers, NT::NAME}},

        {NT::Modifyers, {NT::Modifyer, NT::Modifyers}},
        {NT::Modifyers, {NT::Modifyer}},

        {NT::Modifyer, {"inner"s}, Actions::ModifyerInner},
        {NT::Modifyer, {"outer"s}, Actions::ModifyerOuter}, 
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
