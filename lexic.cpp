#include "lexic.h"

const std::set<std::string> keywords = {
    "arguments",
    ":", "(", ")", ";", "{", "}", "=>", ",", "+", "-", "*", "/", "="
};

using namespace Tokens;

Token *LexicAnalys::Get() {
    States state = States::Start, ostate = States::Start;
    std::string buffer;

    if (double_token) {
        Token *ret = double_token;
        double_token = NULL;
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
                        case States::Operator:
                            while (!buffer.empty() && !keywords.count(buffer)) {
                                buffer.pop_back();
                                this->stream->unget();
                            }
                            if (buffer.empty()){
                                throw std::runtime_error("Lexic: Unknown operator");
                            }
                            if (buffer == "}") {  // Fake expression, for ';'
                                double_token = new Token(buffer);
                                return new Token(EMPTY);
                            }
                            return new Token(buffer);
                        default: 
                            buffer = "";
                    }
                }
            break;
            case States::Error: 
                throw std::runtime_error("Lexic: Unexpected symbol");
            case States::End: 
                double_token = new Token(END);  // Fake expression, for ';'
                return new Token(EMPTY);
            break;
            case States::Comment:
            break;
            default:
                buffer += symbol;
        }

        ostate = state;
    }
    
}

