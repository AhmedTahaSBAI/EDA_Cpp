#pragma once
#include <string>

enum TokenType {
    IDENTIFIER,
    KEYWORD,
    NUMBER,
    STRING,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,

    COLON,
    SEMICOLON,
    COMMA,
    DOT,

    ASSIGN_OP,
    PLUS,
    MINUS,
    STAR,
    SLASH,

    OP_AND,
    OP_OR,
    OP_NOT,

    COMMENT,
    INVALID,
    EOF_TOKEN
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::ASSIGN_OP: return "ASSIGN_OP";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::OP_AND: return "OP_AND";
        case TokenType::OP_OR: return "OP_OR";
        case TokenType::OP_NOT: return "OP_NOT";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::INVALID: return "INVALID";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}