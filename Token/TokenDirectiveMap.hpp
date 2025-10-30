
#ifndef TOKEN_DERECTIVE_MAP_HPP
#define TOKEN_DERECTIVE_MAP_HPP
#pragma once

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
    {"#include",       TTokenID::IncludeDirective},
};

#endif // TOKEN_DERECTIVE_MAP_HPP