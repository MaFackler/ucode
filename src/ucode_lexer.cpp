#include <doctest.h>
#include "ucode_lexer.h"


void Lexer::lex(const char *contents, vector<Token> &tokens) {
    tokens.clear();
    if (!contents) {
        return;
    }
    auto set_token_type = [](vector<const char *> &v, const char *a, size_t n, enum TokenType to_set, enum TokenType *value) {
        if (*value != TokenType::NONE) {
            return;
        }
        for (const char *e: v) {
            if (n == strlen(e) && std::strncmp(a, e, n) == 0) {
                *value = to_set;
            }
        }
    };
    auto add_token = [](vector<Token> &tokens, enum TokenType type, const char *start, const char *end) -> const char* {
        auto &t = tokens.emplace_back();
        t.type = type;
        t.chars = string_view(start, end - start);
        return end;
    };
    auto add_word_or_number = [this, set_token_type, add_token](vector<Token> &tokens, bool in_word, bool in_number, const char *start, const char *end) -> const char* {
        auto res = end; 
        if (in_word) {
            enum TokenType type = TokenType::NONE;
            set_token_type(this->keywords, start, end - start, TokenType::KEYWORD, &type);
            set_token_type(this->types, start, end - start, TokenType::TYPE, &type);
            start = add_token(tokens, type, start, end);
        }
        if (in_number) {
            start = add_token(tokens, TokenType::NUMBER, start, end);
        }
        return res;
    };
    bool in_word = false;
    bool in_number = false;
    const char *start = contents;
    while (*contents != 0) {
        char c = *contents++;
        switch (c) {
            case ' ':
                start = add_word_or_number(tokens, in_word, in_number, start, contents - 1);
                start = add_token(tokens, TokenType::WHITESPACE, start, contents);
                in_word = false;
                in_number = false;
                break;
            case '(':
                start = add_word_or_number(tokens, in_word, in_number, start, contents - 1);
                start = add_token(tokens, TokenType::PARENTHESIS, start, contents);
                break;
            case ')':
                start = add_word_or_number(tokens, in_word, in_number, start, contents - 1);
                start = add_token(tokens, TokenType::PARENTHESIS, start, contents);
                in_word = false;
                in_number = false;
                break;
            case '.':
                if (in_number) {
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (!in_word) {
                    in_number = true;
                }
                break;
            case ':':
            case ';':
            case '<':
            case '=':
            case '>':
            case '?':
            case '@':
                start = add_token(tokens, TokenType::NONE, start, contents);
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
                if (!in_number) {
                    in_word = true;
                }
                break;
            case '{':
                start = add_word_or_number(tokens, in_word, in_number, start, contents - 1);
                start = add_token(tokens, TokenType::PARENTHESIS, start, contents);
                break;
            case '}':
                start = add_word_or_number(tokens, in_word, in_number, start, contents - 1);
                start = add_token(tokens, TokenType::PARENTHESIS, start, contents);
                in_word = false;
                in_number = false;
                break;
            case '\0':
                break;
        }
    }

    if (in_word) {
        add_token(tokens, TokenType::WORD, start, contents);
    } else if (in_number) {
        add_token(tokens, TokenType::NUMBER, start, contents);
    }
}

TEST_CASE("FOO") {

    Lexer l; 
    SUBCASE("single word") {
        std::vector<Token> tokens;
        l.lex("Hello", tokens);
        CHECK(tokens.size() == 1);
        CHECK(tokens[0].chars == "Hello");
    }

    SUBCASE("words") {
        std::vector<Token> tokens;
        l.lex("Hello world", tokens);
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].chars == "Hello");
        CHECK(tokens[1].chars == " ");
        CHECK(tokens[2].chars == "world");
    }

    SUBCASE("Numbers") {
        std::vector<Token> tokens;
        l.lex("1 1.0 1.0f abc1", tokens);
        CHECK(tokens.size() == 7);
        CHECK(tokens[0].chars == "1");
        CHECK(tokens[1].chars == " ");
        CHECK(tokens[2].chars == "1.0");
        CHECK(tokens[3].chars == " ");
        CHECK(tokens[4].type == TokenType::NUMBER);
        CHECK(tokens[4].chars == "1.0f");
        CHECK(tokens[5].chars == " ");
        CHECK(tokens[6].type == TokenType::WORD);
        CHECK(tokens[6].chars == "abc1");
    }

    SUBCASE("Function") {
        std::vector<Token> tokens;
        l.lex("void hello(1) {", tokens);
        //CHECK(tokens.size() == 8);
        CHECK(tokens[0].chars == "void");
        CHECK(tokens[1].chars == " ");
        CHECK(tokens[2].chars == "hello");
        CHECK(tokens[3].chars == "(");
        CHECK(tokens[4].chars == "1");
        CHECK(tokens[5].chars == ")");
        CHECK(tokens[6].chars == " ");
    }
    SUBCASE("Keywords") {
        std::vector<Token> tokens;
        l.keywords.emplace_back("if");
        l.lex("if (1)", tokens);
        CHECK(tokens[0].type == TokenType::KEYWORD);
        CHECK(tokens.size() == 5);
    }
    
    SUBCASE("types") {
        std::vector<Token> tokens;
        l.types.emplace_back("int");
        l.lex("int hey = 0;", tokens);
        CHECK(tokens[0].type == TokenType::TYPE);
        CHECK(tokens.size() == 8);
    }


    //SUBCASE("if keyword") {
    //    l.lex("if ifend #if if");
    //    CHECK(l.tokens.size() == 15);
    //}
}
