
#ifndef LEXER_HPP
#define LEXER_HPP
#pragma once

#include "TokenKinds.h"
#include "TokenKeywordMap.hpp"

#include "LexerError.hh"

#include <string>
#include <vector>
#include <unordered_map>

using Callback = void(*)(std::string);
Callback LexError;

#define DEF_GENERATION_BASE(X) \
push_back_token_storage(); \
Token Token{ TokenKind::##X, std::string(1, constexprToChar(TokenKind::##X)), CurrentLine, CurrentColumn }; \
BufferToken.push_back(Token); \

class Lexer {
private:
    using LexEnginePtr = void (Lexer::*)();
private:
    int CurrentLine = 0;
    int CurrentColumn = 0;
    int PosBuffer = 0;
    std::string storage_value = "";

    void LexNumericConstant();
    bool isPreprocessingNumberBody(char C) const;
    char getCharAndSize(const char* ptr, unsigned& size) const;
    const char* consumeChar(const char* ptr, unsigned size);

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

    {'.', &Lexer::LexNumericConstant},
    {'0', &Lexer::LexNumericConstant},
    {'1', &Lexer::LexNumericConstant},
    {'2', &Lexer::LexNumericConstant},
    {'3', &Lexer::LexNumericConstant},
    {'4', &Lexer::LexNumericConstant},
    {'5', &Lexer::LexNumericConstant},
    {'6', &Lexer::LexNumericConstant},
    {'7', &Lexer::LexNumericConstant},
    {'8', &Lexer::LexNumericConstant},
    {'9', &Lexer::LexNumericConstant},

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
        if (((c >= constexprToChar(TokenKind::A) && c <= constexprToChar(TokenKind::Z)) ||
            (c >= constexprToChar(TokenKind::a) && c <= constexprToChar(TokenKind::z)) ||
            c == constexprToChar(TokenKind::Underscore)))
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
    std::vector<Token> BufferToken;
public:
     Lexer(const std::string source) {
        SourceCode = source;
        LexerRun();
    };

    std::vector<Token> GetBufferLexerToken () {
        return BufferToken;
    }
};

void Lexer::LexerRun() {
    while (neof()) {
        char currentChar = GetChar();

        if (auto it = map.find(currentChar); it != map.end()) {
            (this->*it->second)();
        }
        else {
            // Обработка идентификаторов
            size_t start = PosBuffer;
            while (neof() && (is_unicode_identifier_start(GetChar()) ||
                isdigit(GetChar()))) {
                PosBuffer++;
            }

            std::string identifier = SourceCode.substr(start, PosBuffer - start);

            Token Token{
                TokenKind::Literal,
                identifier,
                CurrentLine,
                CurrentColumn
            };
            BufferToken.push_back(Token);
            continue;
        }

        CurrentLine++;
        PosBuffer++;
    }
}

bool Lexer::isPreprocessingNumberBody(char C) const {
    return isalnum(C) || C == '.' || C == '_' || C == '$' ||
        (C >= 0x80); // UTF-8 символы
}

const char* Lexer::consumeChar(const char* ptr, unsigned size) {
    PosBuffer += size;
    return ptr + size;
}

char Lexer::getCharAndSize(const char* ptr, unsigned& size) const {
    if (ptr >= SourceCode.c_str() + SourceCode.size()) {
        size = 0;
        return '\0';
    }
    // Для простоты, но надо учитывать UTF-8
    // Так как isPreprocessingNumberBody есть проверка на этот юникод
    size = 1;
    return *ptr;
}

void Lexer::LexNumericConstant() {
    
    Token token 
    {
        TokenKind::Literal, "",
        CurrentLine, CurrentColumn
    };

    std::string numericValue = "";
    const char* CurPtr = SourceCode.c_str() + PosBuffer;
    unsigned Size = 0;
    char C = getCharAndSize(CurPtr, Size);
    char PrevCh = 0;

    if (C == '.' && PosBuffer + 1 < SourceCode.size() && !isdigit(SourceCode[PosBuffer + 1]))
    {
        token.type = TokenKind::Dot;
        token.value = std::string(1, C);
        BufferToken.push_back(token);
        return;
    }

    // Собираем тело числа
    while (isPreprocessingNumberBody(C)) {
        numericValue += C;
        CurPtr = consumeChar(CurPtr, Size);
        PrevCh = C;
        C = getCharAndSize(CurPtr, Size);
    }

    // Обработка экспоненты: 1e+12, 1e-12
    if ((C == '-' || C == '+') && (PrevCh == 'E' || PrevCh == 'e')) {
        numericValue += C;
        CurPtr = consumeChar(CurPtr, Size);
        C = getCharAndSize(CurPtr, Size);

        // Продолжаем сбор
        while (isPreprocessingNumberBody(C)) {
            numericValue += C;
            CurPtr = consumeChar(CurPtr, Size);
            C = getCharAndSize(CurPtr, Size);
        }
    }

    // Обработка шестнадцатеричной плавающей точки: 0x1.2p+3
    if ((C == '-' || C == '+') && (PrevCh == 'P' || PrevCh == 'p')) {
        numericValue += C;
        CurPtr = consumeChar(CurPtr, Size);
        C = getCharAndSize(CurPtr, Size);

        while (isPreprocessingNumberBody(C)) {
            numericValue += C;
            CurPtr = consumeChar(CurPtr, Size);
            C = getCharAndSize(CurPtr, Size);
        }
    }

    // Обработка разделителей разрядов: 1'000'000
    if (C == '\'' && PosBuffer + 1 < SourceCode.size()) {
        char Next = SourceCode[PosBuffer + 1];
        if (isalnum(Next) || Next == '_') {
            numericValue += C;
            CurPtr = consumeChar(CurPtr, Size);
            C = getCharAndSize(CurPtr, Size);

            while (isPreprocessingNumberBody(C)) {
                numericValue += C;
                CurPtr = consumeChar(CurPtr, Size);
                C = getCharAndSize(CurPtr, Size);
            }
        }
    }

    // Обновляем позицию
    PosBuffer = CurPtr - SourceCode.c_str() - 1;
    token.value = numericValue;
    BufferToken.push_back(token);
}

const char Lexer::GetChar() {
    return SourceCode[PosBuffer];
}

void Lexer::push_back_token_storage() {
    if (!storage_value.empty()) {
        Token Token{ TokenKind::Literal, storage_value, CurrentLine, CurrentColumn };
        BufferToken.push_back(Token);
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