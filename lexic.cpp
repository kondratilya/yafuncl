#include <algorithm>
#include "lexic.h"

using namespace Tokens;
using namespace std::string_literals;

LexicAnalys::Terminals LexicAnalys::terminal(int c) {
    if (c < 0) { 
        return Terminals::Eof; 
    }
    for (auto const& term : terminals) {
        if (term.second.find(static_cast<char>(c)) != std::string::npos)
            return term.first;
    }
    throw std::runtime_error("Lexic: Unknown symbol "s + (char)(c));
}

int LexicAnalys::GetSymbol() {
    if (read_line_.empty()) {
        if (!std::getline(*stream_, read_line_)) {
            return -1;
        }
        line_++;
        column_=0;
        return '\n';
    }
    column_++;
    int c = read_line_.front(); 
    read_line_.erase(0, 1);
    return c;
}

void LexicAnalys::UnGetSymbol(char c) {
    column_--;
    read_line_ = std::string(1, c)+read_line_;
}

void LexicAnalys::SkipRest() {
    read_line_.clear();
}

Token *LexicAnalys::Get() {
    States state = States::Start, ostate = States::Start;
    std::string buffer;

    if (!ungets_.empty()) {
        Token *ret = ungets_.top();
        ungets_.pop();
        return ret;
    }

    while (true) {
        int symbol;

        Terminals term = this->terminal(symbol = GetSymbol());
        state = rules[static_cast<int>(state)][static_cast<int>(term)];

        switch (state) {
            case States::Start: 
                if (ostate != States::Start) {
                    UnGetSymbol(static_cast<char>(symbol));
                    switch (ostate) {
                        case States::Name:
                            if (keywords.count(buffer))
                                return new Token(buffer);
                            return new Token(ID, buffer);
                        case States::Operator: {
                            std::string read_operators = buffer;
                            while (!buffer.empty() && !keywords.count(buffer)) {
                                UnGetSymbol(buffer.back());
                                buffer.pop_back();
                            }
                            if (buffer.empty()){
                                throw std::runtime_error("Lexic: Unknown operator \""s + read_operators + "\"");
                            }
                            if (buffer == "}") {  // Fake expression, for ';'
                                UnGet(new Token(buffer));
                                return new Token(EMPTY);
                            }
                        } return new Token(buffer);
                        default: 
                            buffer = "";
                    }
                }
            break;
            case States::Error: 
                throw std::runtime_error("Lexic: Unexpected symbol");
            case States::End: 
                UnGet(new Token(END));      // Fake expression, for ';'
                return new Token(EMPTY);
            break;
            case States::Comment:
                SkipRest();
             break;
            default:
                buffer += symbol;
        }

        ostate = state;
    }
}

void LexicAnalys::UnGet(std::string id) {
    this->ungets_.push(new Token(id));
}

void LexicAnalys::UnGet(Token*token) {
    this->ungets_.push(token);
}

Token *LexicAnalys::ShowNext() {
    Token *R_next = Get();
    UnGet(R_next);
    return R_next;
}