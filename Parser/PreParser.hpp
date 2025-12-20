#ifndef PRE_PARSER_HPP
#define PRE_PARSER_HPP
#pragma once

#include "TokenID.hpp"

#include <vector>
#include <unordered_map>

class PreParser {
private:
	using LexEnginePtr = std::vector<LexToken>(PreParser::*)();
	int PosBuffer = 0;
	int SizeBufferBasic = 0;
	
private:
	std::vector<LexToken> BufferPreParserToken;
	std::vector<LexToken> BufferPostLexerToken;
	void Init();

	std::unordered_map<std::string, std::vector<LexToken>> ResolvedAlias;

	std::unordered_map<TTokenID, LexEnginePtr> map{ {
	{TTokenID::Using,		 &PreParser::Using},   // Using
	{TTokenID::Type,		 &PreParser::ResolvingType},   // Type
	{TTokenID::Access,		 &PreParser::ResolvingAccess},   // Access
	{TTokenID::Pointer,		 &PreParser::ResolvingPointer},   // Pointer
	} };

	std::vector<LexToken> Using();
	std::vector<LexToken> Resolving();

	std::vector<LexToken> ResolvingType();
	std::vector<LexToken> ResolvingAccess();
	std::vector<LexToken> ResolvingPointer();
	bool neof() {
		return PosBuffer < SizeBufferBasic;
	}

public:
	PreParser(const std::vector<LexToken>& lexbuffer) {
		SizeBufferBasic = lexbuffer.size();
		BufferPreParserToken.reserve(SizeBufferBasic);
		BufferPostLexerToken.reserve(SizeBufferBasic);
		BufferPostLexerToken = lexbuffer;
		Init();
	}

	const std::vector<LexToken>& GetBufferPreParserToken() const {
		return BufferPreParserToken;
	}
};

void PreParser::Init() {
	while (neof()) {
		auto it = map.find(BufferPostLexerToken[PosBuffer].type);

		if (it != map.end())
		{
			auto vec = (this->*it->second)();
			for (auto& i : vec)
				BufferPreParserToken.push_back(i);
		}
		else
			BufferPreParserToken.push_back(BufferPostLexerToken[PosBuffer]);

		PosBuffer++;
	}
};

std::vector<LexToken> PreParser::Using() {

	PosBuffer++; // Пропускаем текущий токен
	PosBuffer++; // Пропускаем отступ

	switch (BufferPostLexerToken[PosBuffer].type)
	{
	case TTokenID::Pointer:
	case TTokenID::Type:
	case TTokenID::Access:
	{
		PosBuffer++; // Пропускаем текущий символ pointer/type/access
		PosBuffer++; // Пропускаем отступ
		std::string Key = BufferPostLexerToken[PosBuffer].value;
		PosBuffer++; // Пропускаем текущее имя ключа
		PosBuffer++; // Пропускаем отступ
		PosBuffer++; // Пропускаем символ '='
		PosBuffer++; // Пропускаем отступ
		std::vector<LexToken> cont;
		while (BufferPostLexerToken[PosBuffer].type != TTokenID::Semicolon) {
			switch (BufferPostLexerToken[PosBuffer].type)
			{
			case TTokenID::Type:
			{
				auto Container = ResolvingType();
				for (const auto& i : Container)
					cont.push_back(i);
				break;
			}
			case TTokenID::Access:
			{
				auto Container = ResolvingAccess();
				for (const auto& i : Container)
					cont.push_back(i);
				break;
			}
			case TTokenID::Pointer:
			{
				auto Container = ResolvingPointer();
				for (const auto& i : Container)
					cont.push_back(i);
				break;
			}
			default:
				break;
			}
			cont.push_back(BufferPostLexerToken[PosBuffer]);
			PosBuffer++;
		}
		if (auto its = ResolvedAlias.find(Key); its == ResolvedAlias.end())
			ResolvedAlias[Key] = cont;
		break;
	}
	default:
		break;
	}
	return std::vector<LexToken>();
};

std::vector<LexToken> PreParser::Resolving() {
	PosBuffer++; // Пропускаем текущий символ type/access/pointer/
	PosBuffer++; // Пропускаем символ '['
	std::string Key = BufferPostLexerToken[PosBuffer].value;
	PosBuffer++; // Пропускаем текущее имя ключа
	PosBuffer++; // Пропускаем символ ']'
	auto its = ResolvedAlias.find(Key);
	if (its == ResolvedAlias.end())
		return std::vector<LexToken>();
	std::vector<LexToken> cont;
	for (const auto& i : its->second)
		cont.push_back(i);
	return cont;
};

std::vector<LexToken> PreParser::ResolvingType() {
	return Resolving();
};

std::vector<LexToken> PreParser::ResolvingAccess() {
	auto cont = Resolving();
	cont.push_back(LexToken{ TTokenID::ScResOp, "::", 0, 0 });
	return cont;
};

std::vector<LexToken> PreParser::ResolvingPointer() {
	auto cont = Resolving();
	cont.push_back(LexToken{ TTokenID::Asterisk, "*", 0, 0 });
	return cont;
};
#endif // PRE_PARSER_HPP