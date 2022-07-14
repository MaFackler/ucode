#pragma once
#include <vector>
#include <string>
#include <cstring>

using std::vector;
using std::string;
using std::string_view;

enum struct TokenType {
    NONE,
    WORD,
    PARENTHESIS,
    WHITESPACE,
    NUMBER,
    KEYWORD,
    TYPE,
};

struct Token {
    enum TokenType type; 
    //string_view chars;
    // TODO: copies!!! I would like to use string_views but it seems the buffer changes
    string chars;
};

// TODO: this is not a lexer? Rename?
struct Lexer {

    vector<const char*> keywords;  //TODO: this is slow????
    vector<const char*> types;
    void lex(const char *contents, vector<Token> &tokens);
};
