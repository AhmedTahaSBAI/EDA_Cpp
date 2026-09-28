#include "parser/Parser.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string extractPropertyValue(const Tokens& tokens, size_t start, size_t end) {
    std::ostringstream out;
    for (size_t i = start; i < end; ++i) {
        const Token& tok = tokens.tokens[i];
        if (i > start && !out.str().empty()) {
            out << ' ';
        }
        out << tok.value;
    }
    return out.str();
}

bool isBlockStart(const Tokens& tokens, size_t index, const std::string& keyword) {
    if (index >= tokens.tokens.size()) {
        return false;
    }

    if (tokens.tokens[index].type != TokenType::KEYWORD || tokens.tokens[index].value != keyword) {
        return false;
    }

    size_t pos = index + 1;
    if (pos >= tokens.tokens.size() || tokens.tokens[pos].type != TokenType::LPAREN) {
        return false;
    }

    ++pos;
    if (pos >= tokens.tokens.size()) {
        return false;
    }

    if (tokens.tokens[pos].type != TokenType::IDENTIFIER && tokens.tokens[pos].type != TokenType::STRING) {
        return false;
    }

    ++pos;
    if (pos >= tokens.tokens.size() || tokens.tokens[pos].type != TokenType::RPAREN) {
        return false;
    }

    ++pos;
    if (pos >= tokens.tokens.size() || tokens.tokens[pos].type != TokenType::LBRACE) {
        return false;
    }

    return true;
}

bool skipBlockBody(const Tokens& tokens, size_t& index) {
    int depth = 0;
    while (index < tokens.tokens.size()) {
        const Token& tok = tokens.tokens[index];
        if (tok.type == TokenType::LBRACE) {
            ++depth;
        } else if (tok.type == TokenType::RBRACE) {
            --depth;
            ++index;
            if (depth <= 0) {
                return true;
            }
            continue;
        }
        ++index;
    }
    return false;
}

} // namespace

// -----------------------------------------------------------------------------
// Parser implementation.
//
// The parser reads tokens emitted by the lexer and rebuilds a higher-level model.
// For a typical Liberty-style file, the pattern is:
//   library { cell { pin { ... } } }
//
// This file currently acts as a structural parser skeleton: it recognizes
// the main blocks and stores the extracted data into the model objects.
// -----------------------------------------------------------------------------

Parser::Parser(const Tokens& tokens)
    : tokens_(tokens), index_(0) {}

// Entry point: walk the token list and parse one or more top-level library blocks.
Library Parser::parse() {
    library_ = Library{};
    index_ = 0;

    while (index_ + 4 < tokens_.tokens.size()) {
        const Token& tok = tokens_.tokens[index_];

        if (tok.type == TokenType::KEYWORD && tok.value == "library" &&
            tokens_.tokens[index_ + 1].type == TokenType::LPAREN &&
            (tokens_.tokens[index_ + 2].type == TokenType::IDENTIFIER ||
             tokens_.tokens[index_ + 2].type == TokenType::STRING) &&
            tokens_.tokens[index_ + 3].type == TokenType::RPAREN &&
            tokens_.tokens[index_ + 4].type == TokenType::LBRACE) {
            library_.name = tokens_.tokens[index_ + 2].value;

            size_t pos = index_ + 5;
            int libDepth = 1;
            while (pos < tokens_.tokens.size() && libDepth > 0) {
                const Token& cur = tokens_.tokens[pos];

                if (cur.type == TokenType::LBRACE) {
                    ++libDepth;
                } else if (cur.type == TokenType::RBRACE) {
                    --libDepth;
                }

                if (libDepth > 0 && cur.type == TokenType::KEYWORD && cur.value == "cell" &&
                    pos + 4 < tokens_.tokens.size() &&
                    tokens_.tokens[pos + 1].type == TokenType::LPAREN &&
                    (tokens_.tokens[pos + 2].type == TokenType::IDENTIFIER ||
                     tokens_.tokens[pos + 2].type == TokenType::STRING) &&
                    tokens_.tokens[pos + 3].type == TokenType::RPAREN &&
                    tokens_.tokens[pos + 4].type == TokenType::LBRACE) {
                    Cell cell;
                    cell.name = tokens_.tokens[pos + 2].value;

                    size_t cellPos = pos + 5;
                    int cellDepth = 1;
                    while (cellPos < tokens_.tokens.size() && cellDepth > 0) {
                        const Token& cellCur = tokens_.tokens[cellPos];

                        if (cellCur.type == TokenType::LBRACE) {
                            ++cellDepth;
                        } else if (cellCur.type == TokenType::RBRACE) {
                            --cellDepth;
                        }

                        if (cellDepth > 0 && cellCur.type == TokenType::KEYWORD && cellCur.value == "pin" &&
                            cellPos + 4 < tokens_.tokens.size() &&
                            tokens_.tokens[cellPos + 1].type == TokenType::LPAREN &&
                            (tokens_.tokens[cellPos + 2].type == TokenType::IDENTIFIER ||
                             tokens_.tokens[cellPos + 2].type == TokenType::STRING) &&
                            tokens_.tokens[cellPos + 3].type == TokenType::RPAREN &&
                            tokens_.tokens[cellPos + 4].type == TokenType::LBRACE) {
                            Pin pin;
                            pin.name = tokens_.tokens[cellPos + 2].value;

                            size_t pinPos = cellPos + 5;
                            int pinDepth = 1;
                            while (pinPos < tokens_.tokens.size() && pinDepth > 0) {
                                const Token& pinCur = tokens_.tokens[pinPos];

                                if (pinCur.type == TokenType::LBRACE) {
                                    ++pinDepth;
                                } else if (pinCur.type == TokenType::RBRACE) {
                                    --pinDepth;
                                }

                                if (pinCur.type == TokenType::KEYWORD && pinCur.value == "direction") {
                                    size_t dirPos = pinPos + 1;
                                    while (dirPos < tokens_.tokens.size() &&
                                           tokens_.tokens[dirPos].type != TokenType::COLON) {
                                        ++dirPos;
                                    }
                                    if (dirPos + 1 < tokens_.tokens.size() &&
                                        (tokens_.tokens[dirPos + 1].type == TokenType::IDENTIFIER ||
                                         tokens_.tokens[dirPos + 1].type == TokenType::KEYWORD ||
                                         tokens_.tokens[dirPos + 1].type == TokenType::STRING)) {
                                        pin.direction = tokens_.tokens[dirPos + 1].value;
                                    }
                                }

                                if ((pinCur.type == TokenType::IDENTIFIER || pinCur.type == TokenType::KEYWORD) &&
                                    pinPos + 1 < tokens_.tokens.size() &&
                                    tokens_.tokens[pinPos + 1].type == TokenType::COLON) {
                                    size_t valueStart = pinPos + 2;
                                    size_t valueEnd = valueStart;
                                    while (valueEnd < tokens_.tokens.size() &&
                                           tokens_.tokens[valueEnd].type != TokenType::SEMICOLON) {
                                        ++valueEnd;
                                    }
                                    if (valueEnd < tokens_.tokens.size()) {
                                        pin.properties[pinCur.value] = extractPropertyValue(tokens_, valueStart, valueEnd);
                                        pinPos = valueEnd;
                                        continue;
                                    }
                                }

                                ++pinPos;
                            }

                            cell.pins.push_back(pin);
                            cellPos = pinPos;
                            continue;
                        }

                        if ((cellCur.type == TokenType::IDENTIFIER || cellCur.type == TokenType::KEYWORD) &&
                            cellPos + 1 < tokens_.tokens.size() &&
                            tokens_.tokens[cellPos + 1].type == TokenType::COLON) {
                            size_t valueStart = cellPos + 2;
                            size_t valueEnd = valueStart;
                            while (valueEnd < tokens_.tokens.size() &&
                                   tokens_.tokens[valueEnd].type != TokenType::SEMICOLON) {
                                ++valueEnd;
                            }
                            if (valueEnd < tokens_.tokens.size()) {
                                cell.properties[cellCur.value] = extractPropertyValue(tokens_, valueStart, valueEnd);
                                cellPos = valueEnd;
                                continue;
                            }
                        }

                        ++cellPos;
                    }

                    library_.cells.push_back(cell);
                    pos = cellPos;
                    continue;
                }

                ++pos;
            }
            break;
        }

        ++index_;
    }

    return library_;
}

// Parse a library block of the form:
//   library(name) {
//       ...
//   }
bool Parser::parseLibrary() {
    if (!expect(TokenType::KEYWORD, "library")) {
        return false;
    }
    if (!expect(TokenType::LPAREN)) {
        return false;
    }
    if (atEnd() || (current().type != TokenType::IDENTIFIER && current().type != TokenType::STRING)) {
        return false;
    }

    library_.name = current().value;
    advance();

    if (!expect(TokenType::RPAREN)) {
        return false;
    }
    if (!expect(TokenType::LBRACE)) {
        return false;
    }

    while (!atEnd()) {
        if (current().type == TokenType::RBRACE) {
            advance();
            break;
        }

        if (current().type == TokenType::EOF_TOKEN) {
            break;
        }

        if (isBlockStart(tokens_, index_, "cell")) {
            if (!parseCell()) {
                return false;
            }
            continue;
        }

        if (current().type == TokenType::KEYWORD && current().value == "timing") {
            size_t save = index_;
            advance();
            if (!skipBlockBody(tokens_, index_)) {
                index_ = save;
            }
            continue;
        }

        advance();
    }

    return true;
}

// Parse a cell block such as:
//   cell(AND2_X1) {
//       pin(A) { direction : input; }
//   }
bool Parser::parseCell() {
    if (!expect(TokenType::KEYWORD, "cell")) {
        return false;
    }
    if (!expect(TokenType::LPAREN)) {
        return false;
    }
    if (atEnd() || (current().type != TokenType::IDENTIFIER && current().type != TokenType::STRING)) {
        return false;
    }

    Cell cell;
    cell.name = current().value;
    advance();

    if (!expect(TokenType::RPAREN)) {
        return false;
    }
    if (!expect(TokenType::LBRACE)) {
        return false;
    }

    while (!atEnd()) {
        if (current().type == TokenType::RBRACE) {
            advance();
            break;
        }

        if (current().type == TokenType::EOF_TOKEN) {
            break;
        }

        if (isBlockStart(tokens_, index_, "pin")) {
            if (!parsePin()) {
                return false;
            }
            continue;
        }

        if (current().type == TokenType::KEYWORD && current().value == "timing") {
            size_t save = index_;
            advance();
            if (!skipBlockBody(tokens_, index_)) {
                index_ = save;
            }
            continue;
        }

        advance();
    }

    library_.cells.push_back(cell);
    return true;
}

// Parse a pin block such as:
//   pin(A) {
//       direction : input;
//   }
bool Parser::parsePin() {
    if (!expect(TokenType::KEYWORD, "pin")) {
        return false;
    }
    if (!expect(TokenType::LPAREN)) {
        return false;
    }
    if (atEnd() || (current().type != TokenType::IDENTIFIER && current().type != TokenType::STRING)) {
        return false;
    }

    Pin pin;
    pin.name = current().value;
    advance();

    if (!expect(TokenType::RPAREN)) {
        return false;
    }
    if (!expect(TokenType::LBRACE)) {
        return false;
    }

    while (!atEnd()) {
        if (current().type == TokenType::RBRACE) {
            advance();
            break;
        }

        if (current().type == TokenType::EOF_TOKEN) {
            break;
        }

        if (current().type == TokenType::KEYWORD && current().value == "direction") {
            advance();
            if (!expect(TokenType::COLON)) {
                return false;
            }
            if (!atEnd() && (current().type == TokenType::IDENTIFIER || current().type == TokenType::KEYWORD || current().type == TokenType::STRING)) {
                pin.direction = current().value;
                advance();
            }
            if (!expect(TokenType::SEMICOLON)) {
                return false;
            }
            continue;
        }

        advance();
    }

    if (!library_.cells.empty()) {
        library_.cells.back().pins.push_back(pin);
    }

    return true;
}

// Generic branch for any property we do not handle explicitly yet.
// Right now it acts as a safe skip mechanism, allowing the parser to continue.
bool Parser::parseProperty() {
    if (current().type == TokenType::EOF_TOKEN) {
        return false;
    }

    if (current().type == TokenType::KEYWORD) {
        return false;
    }

    advance();
    return true;
}

// Expect the current token to have a given type and optional value.
bool Parser::expect(TokenType type, const std::string& value) {
    if (atEnd()) {
        return false;
    }

    if (current().type != type) {
        return false;
    }

    if (!value.empty() && current().value != value) {
        return false;
    }

    advance();
    return true;
}

// Match and consume a token if it is present.
bool Parser::match(TokenType type, const std::string& value) {
    if (atEnd()) {
        return false;
    }

    if (current().type != type) {
        return false;
    }

    if (!value.empty() && current().value != value) {
        return false;
    }

    advance();
    return true;
}

// Checks whether the cursor is at the end of the token stream.
bool Parser::atEnd() const {
    return index_ >= tokens_.tokens.size() || tokens_.tokens[index_].type == TokenType::EOF_TOKEN;
}

// Return the current token without moving the cursor.
const Token& Parser::current() const {
    if (atEnd()) {
        return tokens_.tokens.back();
    }
    return tokens_.tokens[index_];
}

// Look ahead in the token stream.
const Token& Parser::peek(size_t offset) const {
    size_t pos = index_ + offset;
    if (pos >= tokens_.tokens.size()) {
        return tokens_.tokens.back();
    }
    return tokens_.tokens[pos];
}

// Advance one token.
void Parser::advance() {
    if (!atEnd()) {
        ++index_;
    }
}
