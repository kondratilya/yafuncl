#pragma once
#include <istream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <format>

const std::string END = "$$end$$";
const std::string ID = "$$id$$";
const std::string EMPTY = "$$empty$$";

class Token {
    std::string id;
    std::string value;
    public:
    Token(std::string id, std::string value="") {
        this->id = id;
        this->value = value;
    }
    std::string Id(){
        return this->id;
    }
    std::string Value(){
        return this->value;
    }
    bool IsName() {
        return id == ID;
    }
    bool operator== (Token *T) {
        return this->id == T->id;
    }
    bool operator!= (Token *T) {
        return !(*this == T);
    }
};

class LexicAnalys {

    enum class States {
        Start = 0,
        End,
        Error,
        Name,
        Operator,
        Comment1,
        Comment2,

    };
    enum class Terminals {
        Letter = 0,
        Operator,
        Space,
        Slash,
        NewLine,
        Eof,
    };
    std::istream *stream;
    std::vector<std::vector<States>> rules = {
        //Letter            Operator           Space              Slash             NewLine        Eof
        {States::Name     , States::Operator , States::Start   , States::Comment1 , States::Start, States::End  }, // Start,
        {States::Error    , States::Error    , States::Error   , States::Error    , States::Error, States::Error}, // End,
        {States::Error    , States::Error    , States::Error   , States::Error    , States::Error, States::Error}, // Error,
        {States::Name     , States::Start    , States::Start   , States::Start    , States::Start, States::Start}, // Word,
        {States::Start    , States::Start    , States::Start   , States::Start    , States::Start, States::Start}, // Operator,
        {States::Start    , States::Start    , States::Start   , States::Comment2 , States::Start, States::Start}, // Comment1,
        {States::Comment2 , States::Comment2 , States::Comment2, States::Comment2 , States::Start, States::Start}, // Comment2,
    };
    std::map<Terminals, std::string> terminals = {
        {Terminals::Letter, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"}, 
        {Terminals::Operator, "~`!@#$%^&*()[]{}-=+;:|\\.,\"'?<>"}, 
        {Terminals::Space, " \t\r"}, 
        {Terminals::Slash, "/"}, 
        {Terminals::NewLine, "\n"}
    };
    Terminals terminal(int c) {
        if (c < 0) { //!!!!
            return Terminals::Eof;
        }
        for (auto const& term : terminals) {
            if (term.second.find(static_cast<char>(c)) != std::string::npos) return term.first;
        }
        throw std::runtime_error("Lexic: Unknown symbol");
    }
    Token *double_token = NULL;
    public:
    LexicAnalys(std::istream *stream) {
        this->stream = stream;
    };
    Token *Get() {
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
                                return new Token(ID, buffer);
                            case States::Operator:
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
                case States::Comment2:
                break;
                default:
                    buffer += symbol;
            }

            ostate = state;
        }
        
    }

    
};