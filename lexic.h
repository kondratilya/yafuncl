#pragma once
#include <istream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
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
    static inline  std::vector<std::vector<States>> rules = {
        //Letter            Operator           Space             Comment             NewLine        Eof
        {States::Name     , States::Operator , States::Start   , States::Comment  , States::Start, States::End  }, // Start,
        {States::Error    , States::Error    , States::Error   , States::Error    , States::Error, States::Error}, // End,
        {States::Error    , States::Error    , States::Error   , States::Error    , States::Error, States::Error}, // Error,
        {States::Name     , States::Start    , States::Start   , States::Start    , States::Start, States::Start}, // Word,
        {States::Start    , States::Operator , States::Start   , States::Start    , States::Start, States::Start}, // Operator,
        {States::Comment  , States::Comment  , States::Comment , States::Comment  , States::Start, States::Start}, // Comment1,
    };
    static inline std::map<Terminals, std::string> terminals = {
        {Terminals::Letter, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"}, 
        {Terminals::Operator, "~`!@/$%^&*()[]{}-=+;:|\\.,\"'?<>"}, 
        {Terminals::Space, " \t\r"}, 
        {Terminals::Comment, "#"}, 
        {Terminals::NewLine, "\n"}
    };
    std::set<std::string> keywords = {};

    Terminals terminal(int c);

    std::istream *stream;
    std::stack<Token *> ungets_;

    public:
    LexicAnalys(std::istream &stream) { 
        Rules().GetTerminalsList(keywords);
        this->stream = &stream;
    };
    Token *Get();
    Token *ShowNext();
    void UnGet(std::string id);
    void UnGet(Token *token);
};