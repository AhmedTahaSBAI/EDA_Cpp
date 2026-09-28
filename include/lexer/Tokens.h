#pragma once
#include "Token.h"
#include <vector>

class Tokens {
public:
    std::vector<Token> tokens;

    void push_back(const Token& token) {
        tokens.push_back(token);
    }

    //  REQUIRED for range-based loop
    auto begin() { return tokens.begin(); }
    auto end() { return tokens.end(); }

    auto begin() const { return tokens.begin(); }
    auto end() const { return tokens.end(); }
};