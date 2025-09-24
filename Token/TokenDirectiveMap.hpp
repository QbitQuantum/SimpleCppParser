#ifndef LEXER_TOKEN_DERECTIVEMAP
#define LEXER_TOKEN_DERECTIVEMAP

#include <string>
#include <unordered_map>
#include "TokenID.hpp"

const std::unordered_map<std::string, TTokenID> TokenDirectiveMap = {
    {"#ifdef",      TTokenID::IfDefDirective},
    {"#ifndef",     TTokenID::IfNDefDirective},
    {"#endif",      TTokenID::EndIfDirective},
    {"#define",     TTokenID::DefineDirective},
    {"#undef",      TTokenID::UndefDirective},
    {"#if",         TTokenID::IfDirective},
    {"#else",       TTokenID::ElseDirective},
};

#endif // LEXER_TOKEN_DERECTIVEMAP