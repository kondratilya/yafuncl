#pragma once
#include <istream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include "tokens.h"
#include "rules.h"

using namespace Tokens;

class LexicAnalys {
    enum class States {
        Start = 0,
        End,
        Error,
        Name,
        Operator,
        Comment,
    };
    enum class Terminals {
        Letter = 0,
        Operator,
        Space,
        Comment,
        NewLine,
        Eof,
    };
    const std::vector<std::vector<States>> rules = {
        //Letter            Operator           Space             Comment             NewLine        Eof
        {States::Name     , States::Operator , States::Start   , States::Comment  , States::Start, States::End  }, // Start,
        {States::Error    , States::Error    , States::Error   , States::Error    , States::Error, States::Error}, // End,
        {States::Error    , States::Error    , States::Error   , States::Error    , States::Error, States::Error}, // Error,
        {States::Name     , States::Start    , States::Start   , States::Start    , States::Start, States::Start}, // Word,
        {States::Start    , States::Operator , States::Start   , States::Start    , States::Start, States::Start}, // Operator,
        {States::Comment  , States::Comment  , States::Comment , States::Comment  , States::Start, States::Start}, // Comment1,
    };
    const std::map<Terminals, std::string> terminals = {
        {Terminals::Letter, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"}, 
        {Terminals::Operator, "~`!@/$%^&*()[]{}-=+;:|\\.,\"'?<>"}, 
        {Terminals::Space, " \t\r"}, 
        {Terminals::Comment, "#"}, 
        {Terminals::NewLine, "\n"}
    };
    std::set<std::string> keywords = {};

    Terminals terminal(int c) {
        if (c < 0) { 
            return Terminals::Eof; 
        }
        for (auto const& term : terminals) {
            if (term.second.find(static_cast<char>(c)) != std::string::npos)
                return term.first;
        }
        throw std::runtime_error("Lexic: Unknown symbol");
    }

    std::istream *stream;
    Token *double_token = NULL;

    public:
    LexicAnalys(std::istream &stream) { 
        Rules r;
        r.GetTerminalsList(keywords);
        this->stream = &stream; 
    };
    Token *Get();
};