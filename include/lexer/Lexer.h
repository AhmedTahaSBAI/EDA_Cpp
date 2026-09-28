#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include "Token.h"
#include "Tokens.h"
#include "Keywords.h"
#include "TokenType.h"
#include <fstream>

class Lexer {
public:
    static void Lexer_lib(const std::string& input, Tokens& tokens);
};
