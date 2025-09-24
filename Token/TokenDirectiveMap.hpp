
#ifndef LEXER_TOKEN_DERECTIVEMAP
#define LEXER_TOKEN_DERECTIVEMAP

#include "TokenID.hpp"

#include <string>
#include <unordered_map>

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