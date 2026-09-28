#pragma once

#include <map>
#include <string>
#include <vector>

#include "lexer/Token.h"
#include "lexer/Tokens.h"

// -----------------------------------------------------------------------------
// Model layer: this is the data structure produced by the parser.
// It represents the meaningful information extracted from a Liberty-like .lib file.
// -----------------------------------------------------------------------------

struct Pin {
    std::string name;            // Example: "A", "Z", "CLK"
    std::string direction;       // Example: "input", "output"
    std::vector<std::string> values; // Additional extracted properties
    std::map<std::string, std::string> properties; // Liberty attributes like capacitance, function, max_capacitance
};

struct Cell {
    std::string name;            // Example: "AND2_X1"
    std::vector<Pin> pins;       // Pins contained in this cell
    std::map<std::string, std::string> properties; // Liberty attributes like area, cell_leakage_power, function
};

struct Library {
    std::string name;            // Example: "my_library"
    std::vector<Cell> cells;     // Cells contained in the library
};

// -----------------------------------------------------------------------------
// Parser: consumes a stream of tokens produced by the lexer and builds the model.
// The main idea is:
//   lexer  -> tokenizes the raw text
//   parser -> interprets tokens and creates C++ objects from them
//   model  -> the structured data used by the rest of the application
// -----------------------------------------------------------------------------
class Parser {
public:
    // Constructor receives the token list produced by the lexer.
    explicit Parser(const Tokens& tokens);

    // Main entry point. It parses the whole token stream and returns the library model.
    Library parse();

private:
    // Parse a "library(...) { ... }" block.
    bool parseLibrary();

    // Parse a "cell(...) { ... }" block.
    bool parseCell();

    // Parse a "pin(...) { ... }" block and its properties.
    bool parsePin();

    // Generic property handler used when we do not yet know the exact type.
    bool parseProperty();

    // Helper methods for token matching and consumption.
    bool expect(TokenType type, const std::string& value = "");
    bool match(TokenType type, const std::string& value = "");
    bool atEnd() const;
    const Token& current() const;
    const Token& peek(size_t offset = 1) const;
    void advance();

    const Tokens& tokens_;  // Reference to the token list from the lexer.
    size_t index_;          // Current position in the token stream.
    Library library_;       // Result model being built.
    std::vector<std::string> errors_; // Small error collection for debugging.
};
