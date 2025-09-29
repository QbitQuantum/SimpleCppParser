
#include "TokenID.hpp"
#include "TokenKeywordMap.hpp"

#include "LexerError.hh"

#include <string>
#include <vector>
#include <utility>
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

struct LexToken {
    TTokenID type;
    std::string value;
    size_t line = 0;
    size_t column = 0;

    bool operator == (const LexToken& other) const {
        return type == other.type;
    };
};

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
    {' ', &LexerEngineBasic::ProcessSpace},
    {'"', &LexerEngineBasic::ProcessQuotation},
    {'#', &LexerEngineBasic::ProcessHash},
    {'$', &LexerEngineBasic::ProcessDollar},
    {'%', &LexerEngineBasic::ProcessPercent},
    {'&', &LexerEngineBasic::ProcessAmpersand},
    {'(', &LexerEngineBasic::ProcessLeftParen},
    {')', &LexerEngineBasic::ProcessRightParen},
    {'{', &LexerEngineBasic::ProcessLeftBrace},
    {'}', &LexerEngineBasic::ProcessRightBrace},
    {'[', &LexerEngineBasic::ProcessLeftBracket},
    {']', &LexerEngineBasic::ProcessRightBracket},
    {'+', &LexerEngineBasic::ProcessPlus},
    {',', &LexerEngineBasic::ProcessComma},
    {'-', &LexerEngineBasic::ProcessMinus},
    {'.', &LexerEngineBasic::ProcessDot},
    {':', &LexerEngineBasic::ProcessColon},
    {';', &LexerEngineBasic::ProcessSemicolon},
    {'<', &LexerEngineBasic::ProcessLess},
    {'=', &LexerEngineBasic::ProcessEquals},
    {'>', &LexerEngineBasic::ProcessGreater},
    {'@', &LexerEngineBasic::ProcessAt},
    {'\\', &LexerEngineBasic::ProcessBackslash},
    {'^', &LexerEngineBasic::ProcessCaret},
    {'`', &LexerEngineBasic::ProcessBacktick},
    {'|', &LexerEngineBasic::ProcessPipe},

    {'\n', &LexerEngineBasic::ProcessLineFeed},
    {'\r', &LexerEngineBasic::ProcessCarriageReturn},
    {'*', &LexerEngineBasic::ProcessAsterisk},
    {'\'', &LexerEngineBasic::ProcessApostrophe},
    {'/', &LexerEngineBasic::ProcessSlash},
    }};


    void ProcessSpace() { DEF_GENERATION_BASE(Space);};
    void ProcessLineFeed() { DEF_GENERATION_BASE(LineFeed);};

    void ProcessHash() { DEF_GENERATION_BASE(Hash); };
    void ProcessDollar() { DEF_GENERATION_BASE(Dollar); };
    void ProcessPercent() { DEF_GENERATION_BASE(Percent); };
    void ProcessAmpersand() { DEF_GENERATION_BASE(Ampersand); };

    void ProcessRightParen() { DEF_GENERATION_BASE(RightParen); };
    void ProcessLeftParen() { DEF_GENERATION_BASE(LeftParen); };

    void ProcessRightBrace() { DEF_GENERATION_BASE(RightBrace); };
    void ProcessLeftBrace() { DEF_GENERATION_BASE(LeftBrace); };

    void ProcessLeftBracket() { DEF_GENERATION_BASE(LeftBracket); };
    void ProcessRightBracket() { DEF_GENERATION_BASE(RightBracket); };

    void ProcessPlus() { DEF_GENERATION_BASE(Plus); };
    void ProcessComma() { DEF_GENERATION_BASE(Comma); };
    void ProcessMinus() { DEF_GENERATION_BASE(Minus); };
    void ProcessDot() { DEF_GENERATION_BASE(Dot); };
    void ProcessColon() { DEF_GENERATION_BASE(Colon); };
    void ProcessSemicolon() { DEF_GENERATION_BASE(Semicolon); };
    void ProcessLess() { DEF_GENERATION_BASE(Less); };
    void ProcessEquals() { DEF_GENERATION_BASE(Equals); };
    void ProcessGreater() { DEF_GENERATION_BASE(Greater); };
    void ProcessAt() { DEF_GENERATION_BASE(At); };
    void ProcessBackslash() { DEF_GENERATION_BASE(Backslash); };
    void ProcessCaret() { DEF_GENERATION_BASE(Caret); };
    void ProcessBacktick() { DEF_GENERATION_BASE(Backtick); };
    void ProcessPipe() { DEF_GENERATION_BASE(Pipe); };
    void ProcessTilde() { DEF_GENERATION_BASE(Tilde); };
    void ProcessAsterisk() { DEF_GENERATION_BASE(Asterisk); };
    void ProcessApostrophe() { DEF_GENERATION_BASE(Apostrophe); };

    void ProcessCarriageReturn();
    void ProcessSlash();
    void ProcessQuotation();

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
                    IsKeyword ? it->second : TTokenID::Identifier,
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
        LexToken LexToken{ TTokenID::Identifier, storage_value, CurrentLine, CurrentColumn };
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

void LexerEngineBasic::ProcessCarriageReturn() {
    push_back_token_storage();
    // Если после `\r` идёт `\n` (Windows: `\r\n`), пропускаем `\n`
    if (PosBuffer + 1 < SourceCode.size() && SourceCode[PosBuffer + 1] == '\n') {
        PosBuffer++;  // Пропускаем `\n`, чтобы не дублировать LineFeed
    }
}

void LexerEngineBasic::ProcessSlash() {
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
        while (neof() && GetChar() != '*') {
            PosBuffer++;
        }
        //Пропускаем '*'
        PosBuffer++;
        //Пропускаем '/'
        PosBuffer++;
    }
    else
    {
        PosBuffer--;
        DEF_GENERATION_TO_CHAR(TTokenID::Slash);
    }
}

void LexerEngineBasic::ProcessQuotation() {
    push_back_token_storage(); 

    LexToken DefLexTokenQuotationBegin{ TTokenID::Quotation, std::string(1, '"'), 0, 0};
    BufferToken.push_back(DefLexTokenQuotationBegin);

    PosBuffer++;
    std::string content = "";
    while (neof() && GetChar() != '"') {
        content += GetChar();
        PosBuffer++;
    }
    LexToken LexTokenQuotation{ TTokenID::Identifier, content, 0, 0 };
    BufferToken.push_back(LexTokenQuotation);

    LexToken DefLexTokenQuotationEnd{ TTokenID::Quotation, std::string(1, '"'), 0, 0 };
    BufferToken.push_back(DefLexTokenQuotationEnd);

    PosBuffer++;
    BufferToken.push_back({ constexprToTTokenID(SourceCode[PosBuffer]), std::string(1, SourceCode[PosBuffer]), 0, 0 });
}