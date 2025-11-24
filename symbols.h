#pragma once
#include <string>
#include "tokens.h"

namespace Symbols {
    enum class NonTerminals {
        ROOT, E,
        T0, T1, T2, T3, T4,
        T5, T6, T7, T8, T9,
        T10, T11, T12, T13,
        LVALUE,           // Может быть слева от =
        NAME,             // Имя переменной
        Program,
        Expressions,
        Expression,
        BlockBegin, Block, 
        FunctionCall, 
        Tuple, List,
        Modifyers, Modifyer,
        If, IfThen, IfElse,
        While, WhileDo, WhileEnd,
        Return, ReturnEmpty,
    };

    class Symbol {
        public:
        std::string name;
        NonTerminals non_terminal;
        bool is_terminal;
        Tokens::Token *token=NULL;

        Symbol(std::string terminal) {
            is_terminal = true;
            this->name = terminal;
        }
        Symbol(NonTerminals non_terminal) {
            is_terminal = false;
            this->non_terminal = non_terminal;
        }
        Symbol(Tokens::Token *token) : Symbol (token->Id()) {
            this->token = token;
        }
        Symbol(const Symbol &s) {
            this->name = s.name;
            this->non_terminal = s.non_terminal;
            this->is_terminal = s.is_terminal;
        }
        ~Symbol() {
            if (token) delete token;
        }
        bool IsTerminal() const { return is_terminal; }
        bool IsName() const { return token && token->IsName(); }
        bool operator== (const std::string &s) const { return IsTerminal() && name == s; }
        bool operator== (const Symbol &S) const {
            if (S.is_terminal == this->is_terminal) {
                if(this->is_terminal) {
                    return this->name == S.name;
                }
                return this->non_terminal == S.non_terminal;
            }
            return false;
        }
        bool operator!= (const Symbol &S) const { return !(*this == S); }
    };

    inline bool operator==(Symbol* s1, const Symbols::Symbol s2){
        return s1->operator==(s2);
    }
}


std::ostream &operator<<(std::ostream &os, Symbols::Symbol &s);
