#include "Lexer.h"
#include <cctype>
#include <fstream>
#include <iostream>

void Lexer::Lexer_lib(const std::string& input, Tokens& tokens) {
    std::ifstream file(input);

    if (!file.is_open()) {
        std::cout << "Error opening file\n";
        return;
    }

    auto peek = [&]() -> int {
        return file.peek();
    };

    char ch;
    int line = 1;
    int column = 0;

    while (file.get(ch)) {
        column++;

        if (ch == '\n') {
            ++line;
            column = 0;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(ch))) {
            continue;
        }

        // Liberty files often use a backslash at the end of a line to continue
        // a long value on the next line. This is not an invalid character.
        if (ch == '\\' && peek() == '\n') {
            file.get(ch); // consume the newline itself
            ++line;
            column = 0;
            continue;
        }

        if (ch == '\\') {
            // In this simplified lexer, a standalone backslash is ignored as a
            // continuation marker when it is not immediately followed by a newline.
            continue;
        }

        Token token;
        token.line = line;
        token.column = column;

        // Handle single-line comments
        if (ch == '/' && peek() == '/') {
            while (file.get(ch) && ch != '\n') {
                column++;
            }
            if (ch == '\n') {
                ++line;
                column = 0;
            }
            continue;
        }

        // Handle block comments: /* ... */
        if (ch == '/' && peek() == '*') {
            int start_col = column;
            std::string comment;
            file.get(ch);
            column++;

            while (file.get(ch)) {
                column++;

                if (ch == '\n') {
                    ++line;
                    column = 0;
                }

                if (ch == '*' && peek() == '/') {
                    file.get(ch);
                    column++;
                    break;
                }

                comment += ch;
            }

            token.type = TokenType::COMMENT;
            token.value = comment;
            token.line = line;
            token.column = start_col;
            tokens.push_back(token);
            continue;
        }

        // Handle identifiers and keywords: library, cell, pin, direction...
        if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
            std::string word;
            int start_col = column;
            word += ch;

            while (peek() != EOF &&
                   (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
                file.get(ch);
                column++;
                word += ch;
            }

            token.value = word;
            token.column = start_col;
            token.type = Keywords.count(word) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
            tokens.push_back(token);
            continue;
        }

        // Handle numeric literals like 1, 0.5, 1e-9
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            std::string num;
            int start_col = column;
            num += ch;

            while (peek() != EOF &&
                   (std::isdigit(static_cast<unsigned char>(peek())) ||
                    peek() == '.' ||
                    peek() == 'e' || peek() == 'E' ||
                    peek() == '+' || peek() == '-')) {
                file.get(ch);
                column++;
                num += ch;
            }

            token.type = TokenType::NUMBER;
            token.value = num;
            token.column = start_col;
            tokens.push_back(token);
            continue;
        }

        // Handle string literals delimited by quotes.
        if (ch == '"') {
            std::string str;
            int start_col = column;

            while (file.get(ch)) {
                column++;

                if (ch == '\n') {
                    ++line;
                    column = 0;
                    break;
                }

                if (ch == '\\') {
                    if (file.get(ch)) {
                        column++;
                        str += ch;
                    }
                    continue;
                }

                if (ch == '"') {
                    break;
                }

                str += ch;
            }

            token.type = TokenType::STRING;
            token.value = str;
            token.column = start_col;
            tokens.push_back(token);
            continue;
        }

        // Handle explicit punctuation and operators.
        switch (ch) {
            case '(':
                token.type = TokenType::LPAREN;
                break;
            case ')':
                token.type = TokenType::RPAREN;
                break;
            case '{':
                token.type = TokenType::LBRACE;
                break;
            case '}':
                token.type = TokenType::RBRACE;
                break;
            case '[':
                token.type = TokenType::LBRACKET;
                break;
            case ']':
                token.type = TokenType::RBRACKET;
                break;
            case ':':
                token.type = TokenType::COLON;
                break;
            case ';':
                token.type = TokenType::SEMICOLON;
                break;
            case ',':
                token.type = TokenType::COMMA;
                break;
            case '.':
                token.type = TokenType::DOT;
                break;
            case '=':
                token.type = TokenType::ASSIGN_OP;
                break;
            case '+':
                token.type = TokenType::PLUS;
                break;
            case '-':
                token.type = TokenType::MINUS;
                break;
            case '*':
                token.type = TokenType::STAR;
                break;
            case '/':
                token.type = TokenType::SLASH;
                break;
            case '&':
                token.type = TokenType::OP_AND;
                break;
            case '|':
                token.type = TokenType::OP_OR;
                break;
            case '!':
                token.type = TokenType::OP_NOT;
                break;
            default:
                std::cerr << "Invalid character '" << ch << "' at line " << line
                          << ", column " << column << "\n";
                token.type = TokenType::INVALID;
                token.value = std::string(1, ch);
                tokens.push_back(token);
                continue;
        }

        token.value = std::string(1, ch);
        tokens.push_back(token);
    }

    tokens.push_back({TokenType::EOF_TOKEN, "", line, column});
}
