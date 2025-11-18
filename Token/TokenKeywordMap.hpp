
#ifndef TOKEN_KEYWORD_MAP_HPP
#define TOKEN_KEYWORD_MAP_HPP
#pragma once

#include "TokenID.hpp"

#include <unordered_map>
#include <string>

constexpr char const_tolower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

constexpr uint32_t ConstexprCppHash(const char* str, uint32_t seed = 0) {
    uint32_t hash = seed;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        uint32_t k = static_cast<uint8_t>(str[i]);
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

uint32_t CppHash(const char* str, uint32_t seed = 0) {
    uint32_t hash = seed;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        uint32_t k = static_cast<uint8_t>(str[i]);
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

uint32_t CppHash(const std::string& s, uint32_t seed = 0) {
    uint32_t hash = seed;
    for (char c : s) {
        uint32_t k = static_cast<uint8_t>(c);
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
    {ConstexprCppHash("div"),          TTokenID::Div},
    {ConstexprCppHash("mod"),          TTokenID::Mod},

    // ===== Встроенные значение типов =====
    {ConstexprCppHash("true"),         TTokenID::BooleanLiteral},
    {ConstexprCppHash("false"),        TTokenID::BooleanLiteral},
    {ConstexprCppHash("nullptr"),      TTokenID::NullptrLiteral},

    // ===== Управляющие конструкции =====
    {ConstexprCppHash("if"),           TTokenID::If},
    {ConstexprCppHash("else"),         TTokenID::Else},
    {ConstexprCppHash("case"),         TTokenID::Case},
    {ConstexprCppHash("for"),          TTokenID::For},
    {ConstexprCppHash("while"),        TTokenID::While},
    {ConstexprCppHash("try"),          TTokenID::Try},
    {ConstexprCppHash("catch"),        TTokenID::Catch},
    {ConstexprCppHash("return"),       TTokenID::Return},

    {ConstexprCppHash("break"),        TTokenID::Break},
    {ConstexprCppHash("continue"),     TTokenID::Continue},
    {ConstexprCppHash("switch"),       TTokenID::Switch},
    {ConstexprCppHash("default"),      TTokenID::Default},
    {ConstexprCppHash("new"),          TTokenID::New},
    {ConstexprCppHash("delete"),       TTokenID::Delete_},

    // ===== Объявление типов =====
    {ConstexprCppHash("class"),        TTokenID::Class},
    {ConstexprCppHash("override"),     TTokenID::Override},
    {ConstexprCppHash("virtual"),      TTokenID::Virtual},
    {ConstexprCppHash("final"),        TTokenID::Final},

    // ===== квалификаторы =====
    {ConstexprCppHash("const"),        TTokenID::Const},

    // ===== Модификаторы =====
    {ConstexprCppHash("private"),      TTokenID::Private},
    {ConstexprCppHash("protected"),    TTokenID::Protected},
    {ConstexprCppHash("public"),       TTokenID::Public},
    {ConstexprCppHash("static"),       TTokenID::Static },

    // ===== Прочее =====
    {ConstexprCppHash("__property"),   TTokenID::Property },
    {ConstexprCppHash("__fastcall"),   TTokenID::FastCall },
    {ConstexprCppHash("var"),          TTokenID::Var },
    {ConstexprCppHash("function"),     TTokenID::Function },
    {ConstexprCppHash("access"),       TTokenID::Access },

    {ConstexprCppHash("using"),        TTokenID::Using },
    {ConstexprCppHash("pointer"),      TTokenID::Pointer },
    {ConstexprCppHash("type"),         TTokenID::Type },

};

static_assert(ConstexprCppHash("div") != ConstexprCppHash("mod"));

#endif //TOKEN_KEYWORD_MAP_HPP