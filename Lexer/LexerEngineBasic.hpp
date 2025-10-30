
#ifndef LEXER_ENGINE_BASIC_HPP
#define LEXER_ENGINE_BASIC_HPP
#pragma once

#include "TokenID.hpp"
#include "TokenKeywordMap.hpp"

#include "LexerError.hh"

#include <string>
#include <vector>
#include <unordered_map>

using Callback = void(*)(std::string);
Callback LexError;

#define DEF_GENERATION_TO_CHAR(X) \
push_back_token_storage(); \
LexToken LexToken{ X, std::string(1, constexprToChar(X)), CurrentLine, CurrentColumn }; \
BufferToken.push_back(LexToken); \

#define DEF_GENERATION_BASE(X) \
push_back_token_storage(); \
LexToken LexToken{ TTokenID::##X, std::string(1, constexprToChar(TTokenID::##X)), CurrentLine, CurrentColumn }; \
BufferToken.push_back(LexToken); \

class LexerEngineBasic {
private:
    using LexEnginePtr = void (LexerEngineBasic::*)();
private:
    int CurrentLine = 0;
    int CurrentColumn = 0;
    int PosBuffer = 0;
    std::string storage_value = "";
private:

    std::unordered_map<char, LexEnginePtr> map{ {
    {' ', &LexerEngineBasic::Space},
    {'"', &LexerEngineBasic::Quotation},
    {'#', &LexerEngineBasic::Hash},
    {'$', &LexerEngineBasic::Dollar},
    {'%', &LexerEngineBasic::Percent},
    {'&', &LexerEngineBasic::Ampersand},
    {'(', &LexerEngineBasic::LeftParen},
    {')', &LexerEngineBasic::RightParen},
    {'{', &LexerEngineBasic::LeftBrace},
    {'}', &LexerEngineBasic::RightBrace},
    {'[', &LexerEngineBasic::LeftBracket},
    {']', &LexerEngineBasic::RightBracket},
    {'+', &LexerEngineBasic::Plus},
    {',', &LexerEngineBasic::Comma},
    {'-', &LexerEngineBasic::Minus},
    {'.', &LexerEngineBasic::Dot},
    {':', &LexerEngineBasic::Colon},
    {';', &LexerEngineBasic::Semicolon},
    {'<', &LexerEngineBasic::Less},
    {'=', &LexerEngineBasic::Equals},
    {'>', &LexerEngineBasic::Greater},
    {'@', &LexerEngineBasic::At},
    {'\\', &LexerEngineBasic::Backslash},
    {'^', &LexerEngineBasic::Caret},
    {'`', &LexerEngineBasic::Backtick},
    {'|', &LexerEngineBasic::Pipe},

    {'\n', &LexerEngineBasic::LineFeed},
    {'\r', &LexerEngineBasic::CarriageReturn},
    {'*', &LexerEngineBasic::Asterisk},
    {'\'', &LexerEngineBasic::Apostrophe},
    {'/', &LexerEngineBasic::Slash},
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

    void CarriageReturn();
    void Slash();
    void Quotation();

    // Обновляем проверку идентификаторов:
    bool is_unicode_identifier_start(char32_t c) {
        if (((c >= constexprToChar(TTokenID::A) && c <= constexprToChar(TTokenID::Z)) ||
            (c >= constexprToChar(TTokenID::a) && c <= constexprToChar(TTokenID::z)) ||
            c == constexprToChar(TTokenID::Underscore)))
            return true;
        return (c >= 0xC0); // Все символы выше ASCII
    }
    bool is_digit(char32_t c) {
        return (c >= constexprToChar(TTokenID::Zero) && c <= constexprToChar(TTokenID::Nine));
    }
    bool neof() {
        return PosBuffer < SourceCode.size();
    }

    void parse_number();
    void push_back_token_storage();
    
    const char GetChar();

    void LexerRun();

    std::string SourceCode = "";
    std::vector<LexToken> BufferToken;
public:
     LexerEngineBasic(const std::string source) {
        SourceCode = source; 
        LexerRun();
    };

    std::vector<LexToken> GetBufferLexerBasicToken () {
        return BufferToken;
    }
};

void LexerEngineBasic::LexerRun() {

    while (neof()) {
        if (is_unicode_identifier_start(GetChar()) || is_digit(GetChar())) {
            
            if (is_digit(GetChar()))
            {
                parse_number();
            }
            else
            {
                size_t start = PosBuffer;
                while (neof() && (is_unicode_identifier_start(GetChar()) || is_digit(GetChar()))) {
                    PosBuffer++;
                }

                std::string identifier = SourceCode.substr(start, PosBuffer - start);
                PosBuffer--;

                // Проверяем, является ли собранная строка ключевым словом
                auto it = TokenKeywordMap.find(CppHash(identifier));
                bool IsKeyword = it != TokenKeywordMap.end();

                LexToken LexToken{
                    IsKeyword ? it->second : TTokenID::Literal,
                    identifier,
                    CurrentLine,
                    CurrentColumn };
                BufferToken.push_back(LexToken);

                CurrentLine += identifier.size();
            }
        }
        else
        {

            if (auto it = map.find(GetChar()); it != map.end()) {
                (this->*it->second)();
            }
            else
            {
                storage_value += GetChar();
            }
        }
        CurrentLine++;
        PosBuffer++;
    }

}

const char LexerEngineBasic::GetChar() {
    return SourceCode[PosBuffer];
}

void LexerEngineBasic::push_back_token_storage() {
    if (!storage_value.empty()) {
        LexToken LexToken{ TTokenID::Literal, storage_value, CurrentLine, CurrentColumn };
        BufferToken.push_back(LexToken);
        storage_value = "";
    }
}

void LexerEngineBasic::parse_number() {
    push_back_token_storage();

    size_t start = PosBuffer;
    bool is_hex = false;
    bool is_float = false;

    // Проверяем на шестнадцатеричное число ($FF)
    if (GetChar() == '0' && 
        PosBuffer + 1 < SourceCode.size() &&
        (SourceCode[PosBuffer + 1] == 'x' || SourceCode[PosBuffer + 1] == 'X')) {
        is_hex = true;
        PosBuffer += 2;
    }

    while (neof()) {
        if (is_hex) {
            if (!isxdigit(GetChar())) break;  // Только 0-9, A-F
        }
        else {
            if (GetChar() == '.') {
                if (is_float) break;  // Уже была точка → ошибка
                is_float = true;
            }
            else if (GetChar() == 'e' || GetChar() == 'E') {
                if (PosBuffer + 1 < SourceCode.size() &&
                    (SourceCode[PosBuffer + 1] == '+' || SourceCode[PosBuffer + 1] == '-')) {
                    PosBuffer++;  // Пропускаем знак экспоненты
                }
                is_float = true;
            }
            else if (!is_digit(GetChar())) {
                break;
            }
        }
        PosBuffer++;
    }

    std::string num_str = SourceCode.substr(start, PosBuffer - start);
    TTokenID kind =
        is_float ? TTokenID::FloatLiteral :
        is_hex ? TTokenID::HexLiteral :
        TTokenID::IntegerLiteral;

    LexToken LexToken{ kind, num_str, CurrentLine, CurrentColumn };
    BufferToken.push_back(LexToken);
    CurrentLine += num_str.size();
    PosBuffer--;
}

void LexerEngineBasic::CarriageReturn() {
    push_back_token_storage();
    // Если после `\r` идёт `\n` (Windows: `\r\n`), пропускаем `\n`
    if (PosBuffer + 1 < SourceCode.size() && SourceCode[PosBuffer + 1] == '\n') {
        PosBuffer++;  // Пропускаем `\n`, чтобы не дублировать LineFeed
    }
}

void LexerEngineBasic::Slash() {
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
        DEF_GENERATION_TO_CHAR(TTokenID::Slash);
    }
}

void LexerEngineBasic::Quotation() {
    push_back_token_storage();

    LexToken DefLexTokenQuotationBegin{ TTokenID::Quotation, std::string(1, '"'), 0, 0};
    BufferToken.push_back(DefLexTokenQuotationBegin);

    PosBuffer++;
    std::string content = "";
    while (neof() && GetChar() != '"') {
        content += GetChar();
        PosBuffer++;
    }
    LexToken LexTokenQuotation{ TTokenID::Literal, content, 0, 0 };
    BufferToken.push_back(LexTokenQuotation);

    LexToken DefLexTokenQuotationEnd{ TTokenID::Quotation, std::string(1, '"'), 0, 0 };
    BufferToken.push_back(DefLexTokenQuotationEnd);

    PosBuffer++;
    BufferToken.push_back({ constexprToTTokenID(SourceCode[PosBuffer]), std::string(1, SourceCode[PosBuffer]), 0, 0 });
}

#endif // LEXER_ENGINE_BASIC_HPP