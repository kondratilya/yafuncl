#pragma once
#include <string>
#include <map>

namespace Tokens {
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
        ~Token() {};

        std::string Id() const { return this->id; }
        std::string Value() const { return this->value; }
        bool IsName() const { return id == ID; }
        bool operator== (std::string id) const { return this->id == id; }
        bool operator== (const Token *T) const { return this->id == T->id; }
        bool operator!= (const Token *T) const { return !(*this == T); }
    };
}