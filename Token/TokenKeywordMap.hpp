#ifndef LEXER_TOKEN_KEYWORDMAP
#define LEXER_TOKEN_KEYWORDMAP

#include <unordered_map>
#include "TokenDelphi.hpp"

constexpr char const_tolower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

constexpr uint32_t ConstexprPascalHash(const char* str, uint32_t seed = 0) {
    uint32_t hash = seed;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        uint32_t k = static_cast<uint8_t>(const_tolower(str[i]));  // Игнорируем регистр
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        hash ^= k;
        hash = (hash << 13) | (hash >> 19);
        hash = hash * 5 + 0xe6546b64;
    }
    hash ^= std::char_traits<char>::length(str);
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

uint32_t PascalHash(const char* str, uint32_t seed = 0) {
    uint32_t hash = seed;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        uint32_t k = static_cast<uint8_t>(const_tolower(str[i]));  // Игнорируем регистр
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        hash ^= k;
        hash = (hash << 13) | (hash >> 19);
        hash = hash * 5 + 0xe6546b64;
    }
    hash ^= std::char_traits<char>::length(str);
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

uint32_t PascalHash(const std::string& s, uint32_t seed = 0) {
    uint32_t hash = seed;
    for (char c : s) {
        uint32_t k = static_cast<uint8_t>(tolower(c));
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        hash ^= k;
        hash = (hash << 13) | (hash >> 19);
        hash = hash * 5 + 0xe6546b64;
    }
    hash ^= s.size();
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

static const std::unordered_map<uint32_t, TTokenID> TokenKeywordMap = {
    // ===== Операторы (расширенный) =====
    // Арифметические
    {ConstexprPascalHash("div"),        {TTokenID::Div}},
    {ConstexprPascalHash("mod"),        {TTokenID::Mod}},

    // Логические/битовые
    {ConstexprPascalHash("and"),        {TTokenID::And}},
    {ConstexprPascalHash("or"),         {TTokenID::Or}},
    {ConstexprPascalHash("not"),        {TTokenID::Not}},
    {ConstexprPascalHash("xor"),        {TTokenID::Xor}},
    {ConstexprPascalHash("Shl"),        {TTokenID::Shl}},
    {ConstexprPascalHash("Shr"),        {TTokenID::Shr}},

    // Сравнения
    {ConstexprPascalHash("<>"),        {TTokenID::NotEqual}},
    {ConstexprPascalHash("<="),        {TTokenID::LessEqual}},
    {ConstexprPascalHash(">="),        {TTokenID::GreaterEqual}},

    // ===== Управляющие конструкции =====
    {ConstexprPascalHash("if"),           {TTokenID::If}},
    {ConstexprPascalHash("else"),         {TTokenID::Else}},
    {ConstexprPascalHash("case"),         {TTokenID::Case}},
    {ConstexprPascalHash("for"),          {TTokenID::For}},
    {ConstexprPascalHash("while"),        {TTokenID::While}},
    {ConstexprPascalHash("do"),           {TTokenID::Do}},
    {ConstexprPascalHash("try"),          {TTokenID::Try}},
    {ConstexprPascalHash("catch"),          {TTokenID::Catch}},
    {ConstexprPascalHash("return"),          {TTokenID::Return}},

    // ===== Объявление типов =====
    {ConstexprPascalHash("const"),       {TTokenID::Const}},
    {ConstexprPascalHash("class"),       {TTokenID::Class}},

    // ===== Процедуры/функции =====
    {ConstexprPascalHash("override"),     {TTokenID::Override}},
    {ConstexprPascalHash("virtual"),      {TTokenID::Virtual}},

    // ===== Модификаторы =====
    {ConstexprPascalHash("private"),      {TTokenID::Private}},
    {ConstexprPascalHash("protected"),    {TTokenID::Protected}},
    {ConstexprPascalHash("public"),       {TTokenID::Public}},

    // ===== Прочее =====
    {ConstexprPascalHash("property"),     {TTokenID::Property} },
    {ConstexprPascalHash("static"),         TTokenID::Static },
    {ConstexprPascalHash("final"),          TTokenID::Final },

    {ConstexprPascalHash("__fastcall"),     {TTokenID::FastCall} },
};

static_assert(ConstexprPascalHash("div") != ConstexprPascalHash("mod"));

#endif //LEXER_TOKEN_KEYWORDMAP