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

        Terminals term = this->terminal(symbol = this->stream->get());
        state = rules[static_cast<int>(state)][static_cast<int>(term)];

        switch (state) {
            case States::Start: 
                if (ostate != States::Start) {
                    this->stream->unget();
                    switch (ostate) {
                        case States::Name:
                            if (keywords.count(buffer))
                                return new Token(buffer);
                            return new Token(ID, buffer);
                        case States::Operator: {
                            std::string read_operators = buffer;
                            while (!buffer.empty() && !keywords.count(buffer)) {
                                buffer.pop_back();
                                this->stream->unget();
                            }
                            if (buffer.empty()){
                                throw std::runtime_error("Lexic: Unknown operator "s + read_operators);
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
                while (!stream->eof() && stream->get()!='\n'); 
                this->stream->unget();
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