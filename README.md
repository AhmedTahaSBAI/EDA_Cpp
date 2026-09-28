# EDA C++ Liberty Parser

A lightweight C++ project for tokenizing and parsing Liberty-format library files used in electronic design automation (EDA). The project reads a `.lib` file, extracts structural information for libraries, cells, and pins, and writes a simplified summary to a text report.

## Overview

This repository includes:

- A lexer for Liberty-like text input
- A parser that recognizes `library`, `cell`, and `pin` blocks
- Data structures for the parsed model
- A sample entry point that reads the Nangate OpenCell library and exports a summary

## Project goals

The current implementation focuses on:

- converting source text into tokens
- identifying important Liberty constructs
- extracting useful metadata such as:
  - library name
  - cell name
  - pin name and direction
  - area
  - leakage power
  - capacitance
  - related power pin

## Repository layout

- `main.cpp` — executable entry point
- `include/lexer/` — lexer-related headers
- `include/parser/` — parser definitions and model structures
- `src/lexer/` — lexer implementation and keyword table
- `src/parser/` — parser implementation
- `files/` — sample input files, 
- `tools/` — small utility scripts
- `parsed_liberty.txt` — generated output from a sample parse run

## Build

From the project root:

```bash
g++ -std=c++17 main.cpp src/lexer/Lexer.cpp src/lexer/Keywords.cpp src/parser/Parser.cpp -Iinclude -Iinclude/lexer -Iinclude/parser -o lexer_demo
```

This produces the executable `lexer_demo`.

## Run

```bash
./lexer_demo
```

By default, the program reads:

```text
files/NangateOpenCellLibrary_typical_ccs.lib
```

and writes the parsed summary to:

```text
parsed_liberty.txt
```

## Example output

The generated report includes a library summary and per-cell details, such as:

```text
Library name: NangateOpenCellLibrary
Cell count: 134

Cell: AND2_X1 (pins: 3)
  area=1.064000
  cell_leakage_power=25.066064
  Pin: A1 direction=input capacitance=0.918145 related_power_pin=VDD
```

## Notes

- This is a focused parser prototype rather than a full industrial-strength Liberty parser.
- It demonstrates a practical tokenization + structural parsing workflow for EDA library files.
- The parser is useful as a base for further enhancements, such as richer property extraction and more complete support for Liberty syntax.

## Requirements

- C++17 compiler
- GNU g++ or compatible compiler
- Linux environment (project was validated in this workspace)
