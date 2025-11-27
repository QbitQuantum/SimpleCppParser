
#ifndef LEXER_HPP
#define LEXER_HPP
#pragma once

#include "TokenID.hpp"
#include "TokenKeywordMap.hpp"

#include "LexerError.hh"

#include <string>
#include <vector>
#include <unordered_map>

using Callback = void(*)(std::string);
Callback LexError;

#define DEF_GENERATION_BASE(X) \
push_back_token_storage(); \
LexToken LexToken{ TTokenID::##X, std::string(1, constexprToChar(TTokenID::##X)), CurrentLine, CurrentColumn }; \
BufferToken.push_back(LexToken); \

class Lexer {
private:
    using LexEnginePtr = void (Lexer::*)();
private:
    int CurrentLine = 0;
    int CurrentColumn = 0;
    int PosBuffer = 0;
    std::string storage_value = "";
private:

    std::unordered_map<char, LexEnginePtr> map{ {
    {' ', &Lexer::Space},
    {'"', &Lexer::Quotation},
    {'#', &Lexer::Hash},
    {'$', &Lexer::Dollar},
    {'%', &Lexer::Percent},
    {'&', &Lexer::Ampersand},
    {'(', &Lexer::LeftParen},
    {')', &Lexer::RightParen},
    {'{', &Lexer::LeftBrace},
    {'}', &Lexer::RightBrace},
    {'[', &Lexer::LeftBracket},
    {']', &Lexer::RightBracket},
    {'+', &Lexer::Plus},
    {',', &Lexer::Comma},
    {'-', &Lexer::Minus},
    {'.', &Lexer::Dot},
    {':', &Lexer::Colon},
    {';', &Lexer::Semicolon},
    {'<', &Lexer::Less},
    {'=', &Lexer::Equals},
    {'>', &Lexer::Greater},
    {'@', &Lexer::At},
    {'\\', &Lexer::Backslash},
    {'^', &Lexer::Caret},
    {'`', &Lexer::Backtick},
    {'|', &Lexer::Pipe},

    {'\n', &Lexer::LineFeed},
    {'\r', &Lexer::CarriageReturn},
    {'*', &Lexer::Asterisk},
    {'\'', &Lexer::Apostrophe},
    {'/', &Lexer::Slash},
    }};


    void Space() { DEF_GENERATION_BASE(Space);};
    void LineFeed() { DEF_GENERATION_BASE(LineFeed);};

    void Hash() { DEF_GENERATION_BASE(Hash); };
    void Dollar() { DEF_GENERATION_BASE(Dollar); };
    void Percent() { DEF_GENERATION_BASE(Percent); };
    void Ampersand() { DEF_GENERATION_BASE(Ampersand); };

    void RightParen() { DEF_GENERATION_BASE(RightParen); };
    void LeftParen() { DEF_GENERATION_BASE(LeftParen); };

    void RightBrace() { DEF_GENERATION_BASE(RightBrace); };
    void LeftBrace() { DEF_GENERATION_BASE(LeftBrace); };

    void LeftBracket() { DEF_GENERATION_BASE(LeftBracket); };
    void RightBracket() { DEF_GENERATION_BASE(RightBracket); };

    void Plus() { DEF_GENERATION_BASE(Plus); };
    void Comma() { DEF_GENERATION_BASE(Comma); };
    void Minus() { DEF_GENERATION_BASE(Minus); };
    void Dot() { DEF_GENERATION_BASE(Dot); };
    void Colon() { DEF_GENERATION_BASE(Colon); };
    void Semicolon() { DEF_GENERATION_BASE(Semicolon); };
    void Less() { DEF_GENERATION_BASE(Less); };
    void Equals() { DEF_GENERATION_BASE(Equals); };
    void Greater() { DEF_GENERATION_BASE(Greater); };
    void At() { DEF_GENERATION_BASE(At); };
    void Backslash() { DEF_GENERATION_BASE(Backslash); };
    void Caret() { DEF_GENERATION_BASE(Caret); };
    void Backtick() { DEF_GENERATION_BASE(Backtick); };
    void Pipe() { DEF_GENERATION_BASE(Pipe); };
    void Tilde() { DEF_GENERATION_BASE(Tilde); };
    void Asterisk() { DEF_GENERATION_BASE(Asterisk); };
    void Apostrophe() { DEF_GENERATION_BASE(Apostrophe); };
    void Quotation() { DEF_GENERATION_BASE(Quotation);};

    void CarriageReturn();
    void Slash();

    // Обновляем проверку идентификаторов:
    bool is_unicode_identifier_start(char32_t c) {
        if (((c >= constexprToChar(TTokenID::A) && c <= constexprToChar(TTokenID::Z)) ||
            (c >= constexprToChar(TTokenID::a) && c <= constexprToChar(TTokenID::z)) ||
            c == constexprToChar(TTokenID::Underscore)))
            return true;
        return (c >= 0xC0); // Все символы выше ASCII
    }

    bool neof() {
        return PosBuffer < SourceCode.size();
    }

    void push_back_token_storage();
    
    const char GetChar();

    void LexerRun();

    std::string SourceCode = "";
    std::vector<LexToken> BufferToken;
public:
     Lexer(const std::string source) {
        SourceCode = source;
        LexerRun();
    };

    std::vector<LexToken> GetBufferLexerToken () {
        return BufferToken;
    }
};

void Lexer::LexerRun() {

    while (neof()) {
        
        if (auto it = map.find(GetChar()); it != map.end()) {
            (this->*it->second)();
        }
        else
        {   
            size_t start = PosBuffer;
            while (neof() && (is_unicode_identifier_start(GetChar()) || isdigit(GetChar()))) {
                PosBuffer++;
            }

            std::string identifier = SourceCode.substr(start, PosBuffer - start);
            PosBuffer--;

            // Проверяем, является ли собранная строка ключевым словом
            auto itKeywordMap = TokenKeywordMap.find(CppHash(identifier));
            bool IsKeyword = itKeywordMap != TokenKeywordMap.end();

            LexToken LexToken{
                IsKeyword ? itKeywordMap->second : TTokenID::Literal,
                identifier,
                CurrentLine,
                CurrentColumn };
            BufferToken.push_back(LexToken);

            CurrentLine += identifier.size();
        }
        CurrentLine++;
        PosBuffer++;
    }

}

const char Lexer::GetChar() {
    return SourceCode[PosBuffer];
}

void Lexer::push_back_token_storage() {
    if (!storage_value.empty()) {
        LexToken LexToken{ TTokenID::Literal, storage_value, CurrentLine, CurrentColumn };
        BufferToken.push_back(LexToken);
        storage_value = "";
    }
}

void Lexer::CarriageReturn() {
    push_back_token_storage();
    // Если после `\r` идёт `\n` (Windows: `\r\n`), пропускаем `\n`
    if (PosBuffer + 1 < SourceCode.size() && SourceCode[PosBuffer + 1] == '\n') {
        PosBuffer++;  // Пропускаем `\n`, чтобы не дублировать LineFeed
    }
}

void Lexer::Slash() {
    push_back_token_storage();
    PosBuffer++;
    char _getchar = GetChar();
    if (_getchar == '/')
    {
        while (neof() && GetChar() != '\n') {
            PosBuffer++;
        }
    }
    else if (_getchar == '*')
    {
        PosBuffer++;
        while (neof()) {
            char current = GetChar();
            PosBuffer++;

            // Ищем последовательность */
            if (current == '*' && neof() && GetChar() == '/') {
                PosBuffer++; // Пропускаем '/'
                break; // Конец комментария
            }
        }
    }
    else
    {
        PosBuffer--;
        DEF_GENERATION_BASE(Slash);
    }
}
#endif // LEXER_HPP