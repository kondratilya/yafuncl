#pragma once
#include <iostream>
#include <istream>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "lexic.h"

#define VERBOSE true

enum class OperationTypes {
    Empty,
    Operation,
    NameLookup,
    Address,
    Tuple,
};

enum class Actions {
    None=0, Name, Equate, Print,
    BeginFunction, EndFunction, Return, Plus, Minus, Multiply, Divide, Negative, 
    Pop,
    Tuple, TupleEmpty, TupleOne, TupleStart, TupleEnd,
    FunctionDefinition, 
    FunctionCall, FunctionCallArg,
};


enum class Operations {
    Add, Equate, Return, Jump, Call, CallArg, Pop, Print
};
std::map<Operations, std::string> str_operations = {
    {Operations::Add, "Add"}, 
    {Operations::Equate, "Equate"}, 
    {Operations::Return, "Return"}, 
    {Operations::Jump, "Jump"}, 
    {Operations::Call, "Call"}, 
    {Operations::CallArg, "CallArg"}, 
    {Operations::Pop, "Pop"}, 
    {Operations::Print, "Print"},
};

class Operation {
    public:
    size_t operation;
    OperationTypes type;
    Operation (OperationTypes type, size_t value) {
        this->type = type;
        operation = value;
    }
    Operation (Operations value) {
        this->type = OperationTypes::Operation;
        operation = static_cast<size_t>(value);
    }
    Operation () {
        this->type = OperationTypes::Empty;
        operation = 0;
    }
};
std::ostream &operator<<(std::ostream &os, Operation const &o) { 
    switch (o.type) {
        case OperationTypes::NameLookup: return os << "[" <<o.operation << "]";
        case OperationTypes::Address: return os << "<" <<o.operation << ">";
        case OperationTypes::Empty: return os << "[ ]";
        case OperationTypes::Operation: return os << str_operations[static_cast<Operations>(o.operation)];
        case OperationTypes::Tuple: return os << "(" <<o.operation << ")";
        default: return os << "???";
    }
}

typedef std::vector<Operation*> Code;

enum class NonTerminals {
    S, E,
    T0, T1, T2, T3, T4, V,
    Program,
    Expressions,
    Expression,
    BlockBegin, Block, 
    FunctionDefinition, 
    FunctionCall, 
    Tuple, 
};
std::map<NonTerminals, std::string> str_non_terminals = {
    {NonTerminals::S, "S"}, {NonTerminals::E, "E"}, 
    {NonTerminals::T0, "T0"},  {NonTerminals::T1, "T1"}, {NonTerminals::T2, "T2"}, 
    {NonTerminals::T3, "T3"}, {NonTerminals::T4, "T4"},
    {NonTerminals::V, "V"}, 
    {NonTerminals::Program, "Program"},
    {NonTerminals::Expressions, "Expressions"},
    {NonTerminals::Expression, "Expression"},
    {NonTerminals::BlockBegin, "BlockBegin"}, {NonTerminals::Block, "Block"}, 
    {NonTerminals::FunctionDefinition, "FunctionDefinition"}, 
    {NonTerminals::FunctionCall, "FunctionCall"}, 
    {NonTerminals::Tuple, "Tuple"}, 
};

class Symbol {
    public:

    std::string name;
    NonTerminals non_terminal;
    bool is_terminal;
    Token *token=NULL;
    
    public:

    Symbol(std::string terminal) {
        is_terminal = true;
        this->name = terminal;
    }
    Symbol(NonTerminals non_terminal) {
        is_terminal = false;
        this->non_terminal = non_terminal;
    }
    Symbol(Token *token) : Symbol (token->Id()) {
        this->token = token;
    }

    bool IsTerminal() {
        return is_terminal;
    }

    bool IsName() {
        return token && token->IsName();
    }

    bool IsRoot() {
        return !IsTerminal() && non_terminal == NonTerminals::S;
    }

    bool operator== (std::string s) {
        return IsTerminal() && name == s;
    }

    bool operator== (Symbol *S) {
        if (S->is_terminal == this->is_terminal) {
            if(this->is_terminal) {
                return this->name == S->name;
            }
            return this->non_terminal == S->non_terminal;
        }
        return false;
    }

    bool operator!= (Symbol *S) {
        return !(*this == S);
    }
};
std::ostream &operator<<(std::ostream &os, Symbol &s) { 
    if (s.IsTerminal()) {
        if (s.name == ID)
            return os << "[" << s.token->Value() << "]";
        else
            return os << s.name;
    } else  {
        return os << "<" << str_non_terminals[s.non_terminal] << ">";
    }
}

class Rule {
    public:
    Symbol left;
    std::vector<Symbol> right;
    Actions action;
    /*Rule(Symbol *left) {
        //this->left(left);
        //this->right = right;
    }*/
};

/*
S=E;
E=E'+'T|E'-'T;
E=T;
T= T'*'F| T'/'F|F;

F='('E')';
F=per;
F='-'T;
*/
class SyntaxAnalys {
    LexicAnalys *lexic;
    bool done = false;
    using NT = NonTerminals;
    std::vector<Rule> rules = {
        // {Symbol(NT::S), {Symbol(NT::FunctionCall), Symbol(END)}},

        // {Symbol(NT::FunctionCall), {Symbol(">"), Symbol(NT::V)},
        // {Symbol(NT::FunctionCall), {Symbol(">"), Symbol(NT::Block)},
        // {Symbol(NT::FunctionCall), {Symbol(">"), Symbol(NT::V)},
        // {Symbol(NT::FunctionCall), {Symbol(">"), Symbol(NT::Block)},

// Program -> FunctionCall '$'
// FunctionCall -> '>' Name | '>' Block | '>' Name Tuple | '>' Block Tuple
// Block -> BlockBegin Expressions '}' | BlockBegin '}'
// BlockBegin -> Tuple '{' | '{'
// Expressions -> Expression Expressions  | Expression
// Tuple -> Expression, Tuple | Expression
// Expression -> V

        {Symbol(NT::S), {Symbol(NT::Program), Symbol(END)}},

        {Symbol(NT::Program), {Symbol(NT::Expressions)}},
        {Symbol(NT::Expressions), {Symbol(NT::Expression), Symbol(";"), Symbol(NT::Expressions)}},
        {Symbol(NT::Expressions), {Symbol(NT::Expression)}},

        {Symbol(NT::Expression), {Symbol(NT::E)}, Actions::Pop},
        {Symbol(NT::Expression), {Symbol(EMPTY)}},
        {Symbol(NT::BlockBegin), {Symbol("{")}, Actions::BeginFunction},
        {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol(NT::Expressions), Symbol("}")}, Actions::EndFunction},
        {Symbol(NT::Block), {Symbol(NT::BlockBegin), Symbol("}")}, Actions::EndFunction},

        {Symbol(NT::FunctionDefinition), {Symbol(NT::Block)}},
        {Symbol(NT::FunctionCall), {Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCall},
        {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol("=>"), Symbol(NT::T3)}, Actions::FunctionCallArg},
        // {Symbol(NT::FunctionCall), {Symbol(">"), Symbol(NT::V)}, Actions::FunctionCall},
        // {Symbol(NT::FunctionCall), {Symbol(NT::E), Symbol(">"), Symbol(NT::V)}, Actions::FunctionCall},

        
        //{Symbol(NT::Tuple), {Symbol("("), Symbol(NT::E), Symbol(","), Symbol(")")}, Actions::Tuple1},
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
        
        {Symbol(NT::V), {Symbol(ID)}, Actions::Name},
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

    std::vector<Symbol*> stack;
    std::vector<std::string> names_lookup;
    std::stack<Token*> names_stack;
    std::stack<size_t> function_definitions;
    std::stack<size_t> tuples_stack;

    void RunAction(size_t rule_id) {
        switch (rules[rule_id].action) {
            case Actions::Name: {
                std::string name = names_stack.top()->Value();
                auto it = std::find(names_lookup.begin(), names_lookup.end(), name);
                int name_index;
                if (it == names_lookup.end()) {
                    names_lookup.push_back(name);
                    name_index = names_lookup.size()-1;
                } else {
                    name_index = std::distance(names_lookup.begin(), it);
                }
                code->push_back(new Operation(OperationTypes::NameLookup, name_index));
                names_stack.pop();
            } break;
            case Actions::TupleEmpty: 
                std::cout << "[[Tuple0]]";
                code->push_back(new Operation(OperationTypes::Tuple, 0));
            break;
            case Actions::TupleOne:
                std::cout << "[[Tuple0]]";
                code->push_back(new Operation(OperationTypes::Tuple, 1));
            break;
            case Actions::TupleStart: {
                tuples_stack.push(2);
            } break;
            case Actions::Tuple: {
                size_t tuple_size = tuples_stack.top()+1;
                tuples_stack.pop();
                tuples_stack.push(tuple_size);
                // Operation *tuple = code->back();
                // if (tuple->type!=OperationTypes::Tuple) {
                //     std::cout << "[["<<rule_id<<"Tuple1]]";
                //     code->push_back(new Operation(OperationTypes::Tuple, 1));
                //     break;
                // }
                // tuple->operation++;
                // std::cout << "[["<<rule_id<<"Tuple" <<tuple->operation << "]]";
            } break;
            case Actions::TupleEnd: {
                code->push_back(new Operation(OperationTypes::Tuple, tuples_stack.top()));
                tuples_stack.pop();
            } break;
            case Actions::Pop: 
                code->push_back(new Operation(Operations::Pop));
            break;            
            case Actions::Plus: 
                code->push_back(new Operation(Operations::Add));
            break;
            case Actions::Equate: 
                code->push_back(new Operation(Operations::Equate));
            break;
            case Actions::Print: 
                code->push_back(new Operation(Operations::Print));
            break;
            case Actions::BeginFunction: 
                function_definitions.push(code->size());
                code->push_back(new Operation());
                code->push_back(new Operation(Operations::Jump));
            break;
            case Actions::EndFunction: { 
                size_t function_start = function_definitions.top();
                function_definitions.pop();
                code->push_back(new Operation(Operations::Return));
                (*code)[function_start]->type = OperationTypes::Address;
                (*code)[function_start]->operation = code->size();
                code->push_back(new Operation(OperationTypes::Address, function_start+2));
            } break;
            case Actions::FunctionCall: 
                code->push_back(new Operation(Operations::Call));
            break;
            case Actions::FunctionCallArg:
                code->push_back(new Operation(Operations::CallArg));
            break;
        }
    }

    bool TestRules(Symbol *S) {
        bool beginning;
        for (size_t i=0; i<stack.size(); ++i){
            for (size_t r=0; r<rules.size(); ++r){
                beginning=true;
                size_t j;

                for (j=i; j<stack.size(); ++j){
                    if (rules[r].right.size()<=j-i || rules[r].right[j-i] != stack[j]){
                        beginning=false;
                        break;
                    }
                }
                if (beginning) {
                    if (rules[r].right.size()>j-i && rules[r].right[j-i] == S) {
                        return true;
                    }
                }
            }
        }
        for (size_t i=0; i<stack.size(); ++i) {
            for (size_t r=0; r<rules.size(); ++r) {
                beginning=true;
                size_t j;

                for (j=i; j<stack.size(); ++j) {
                    if (rules[r].right.size()<=j-i || rules[r].right[j-i] != stack[j]) {
                        beginning=false;
                        break;
                    }
                }
                if (beginning && rules[r].right.size() == stack.size()-i) {
                    if (rules[r].left.IsRoot() && i!=0) {
                        throw std::runtime_error("Syntax: Early end");
                    }
                    stack.resize(i);
                    stack.push_back(&rules[r].left);

                    RunAction(r);

                    return(false);
                }
            }

        }
        if (Symbol(END)==S) {
            throw std::runtime_error("Syntax: Error");
        }
        return(true);

    }

    public:
    Code *code;

    std::string getName(int id) {
        return names_lookup[id];
    }

    SyntaxAnalys(LexicAnalys *lexic) {
        this->lexic = lexic;
        code = new Code;
    };
    bool Analyse() {
        if (done) {
            return true;
        }
        bool to_read=true;
        Token *R;
        Symbol *S;

        // code->push_back(new Operation(Operations::CreateContext));
        while(true){
            if(to_read) {
                R = this->lexic->Get();
                S = new Symbol(R);
                if (S->IsName()) {
                    names_stack.push(R);
                }
            }

            to_read=TestRules(S);
            if(to_read)
                stack.push_back(S);

#ifdef VERBOSE
            for (auto i : stack) {
                std::cout << *i << " ";
            }
            std::cout << "  <-- " << *S << std::endl;
#endif

            if(stack.size()==1 && stack[0]->IsRoot()) {
                // code->push_back(new Operation(Operations::DestroyContext));
                done = true;            
                return true;

                /*if (Token(END)==R) {
                    std::cout << " <OK>";
                    return true;
                } else {
                    throw std::runtime_error("Syntax: Early end");
                    return false;
                }*/
            } 
        }
        return false;
    }
    
};

// (1,);
// 1,2;
// :>{a};
// a=>{};
// :a;
//b=(1>{2});
//:b,
//(12,13,3)
//a=b=c=0;
//:a;:b;:c;
//:x;
//x=(x+a);
//(1,) * 3;

//{:y};
//:x;
//x=x+x;
//:x;
//a={1+1};
//:a

//h={f; v; k};jj;{}


//3 = {inc(2);};
//add = a, b>{a+b;};
//print(add(1, 3));
//print({++3});
//print((a,b){a+b}(1,3));


//    a  o  s  / \n
//S   A  O  S C1  S
//A   A  S  S  S  S
//O1  S O2  S O2  S
//O2  S  S  S  S  S
//C1  S O2  S C2  S
//C2 C2 C2 C2 C2  S
//tuples

// Program -> FunctionCall '$'
// FunctionCall -> '>' Name | '>' Block | '>' Name Tuple | '>' Block Tuple
// Block -> BlockBegin Expressions '}' | BlockBegin '}'
// BlockBegin -> Tuple '{' | '{'
// Expressions -> Expression Expressions  | Expression
// Tuple -> Expression, Tuple | Expression
// Expression -> V