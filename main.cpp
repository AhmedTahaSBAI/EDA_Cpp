#include <fstream>
#include <iostream>
#include <vector>

#include "lexer/Lexer.h"
#include "lexer/TokenType.h"
#include "lexer/Tokens.h"
#include "parser/Parser.h"

int main() {
    Tokens tokens;
    Lexer::Lexer_lib("/home/sbai/Projects/c++/EDA_C++/files/NangateOpenCellLibrary_typical_ccs.lib", tokens);

    Parser parser(tokens);
    Library lib = parser.parse();

    std::ofstream out("parsed_liberty.txt", std::ios::trunc);
    if (!out) {
        std::cerr << "Unable to open parsed_liberty.txt for writing\n";
        return 1;
    }

    out << "Library name: " << lib.name << "\n";
    out << "Cell count: " << lib.cells.size() << "\n\n";

    for (const auto& cell : lib.cells) {
        out << "Cell: " << cell.name << " (pins: " << cell.pins.size() << ")\n";
        if (cell.properties.count("area") > 0) {
            out << "  area=" << cell.properties.at("area") << "\n";
        }
        if (cell.properties.count("cell_leakage_power") > 0) {
            out << "  cell_leakage_power=" << cell.properties.at("cell_leakage_power") << "\n";
        }
        for (const auto& pin : cell.pins) {
            out << "  Pin: " << pin.name << " direction=" << pin.direction;
            if (pin.properties.count("capacitance") > 0) {
                out << " capacitance=" << pin.properties.at("capacitance");
            }
            if (pin.properties.count("related_power_pin") > 0) {
                out << " related_power_pin=" << pin.properties.at("related_power_pin");
            }
            out << "\n";
        }
        out << "\n";
    }

    std::cout << "Parsed Liberty data saved to parsed_liberty.txt\n";
    return 0;
}