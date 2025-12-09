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
	{TTokenID::Type,		 &PreParser::Resolving},   // Type
	{TTokenID::Access,		 &PreParser::Resolving},   // Access
	{TTokenID::Pointer,		 &PreParser::Resolving},   // Pointer
	} };

	std::vector<LexToken> Using();
	std::vector<LexToken> Resolving();

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

	switch (BufferPostLexerToken[PosBuffer].type)
	{
	case TTokenID::Pointer:
	case TTokenID::Type:
	case TTokenID::Access:
	{
		TTokenID Save = BufferPostLexerToken[PosBuffer].type;

		PosBuffer++; // Пропускаем текущий символ pointer/type/access
		PosBuffer++; // Пропускаем символ '['
		std::string Key = BufferPostLexerToken[PosBuffer].value;
		PosBuffer++; // Пропускаем текущее имя ключа
		if (auto its = ResolvedAlias.find(Key); its != ResolvedAlias.end())
		{
			switch (Save)
			{
			case TTokenID::Type:
			{
				std::vector<LexToken> cont;
				for (const auto& i : its->second)
					cont.push_back(i);
				return cont;
			};
			case TTokenID::Access:
			{
				std::vector<LexToken> cont;
				LexToken TokenSpace = BufferPostLexerToken[PosBuffer];
				PosBuffer++; // Пропускаем отступ
				cont.push_back(TokenSpace);
				LexToken Identifier = BufferPostLexerToken[PosBuffer];
				for (const auto& i : its->second)
					cont.push_back(i);
				cont.push_back(LexToken{ TTokenID::ScResOp, "::", 0, 0 });
				cont.push_back(Identifier);
				return cont;
			}
			case TTokenID::Pointer:
			{
				std::vector<LexToken> cont;

				for (size_t i = 0; i < its->second.size(); i++)
				{
					switch (its->second[i].type)
					{
					case TTokenID::Type:
					{
						i++; // Пропускаем текущий символ type
						i++; // Пропускаем символ '['
						std::string Key = its->second[i].value;
						i++; // Пропускаем текущее имя ключа
						if (auto its1 = ResolvedAlias.find(Key); its1 != ResolvedAlias.end())
							for (const auto& i : its1->second)
								cont.push_back(i);
						break;
					}
					case TTokenID::Access:
					{
						LexToken TokenSpace = BufferPostLexerToken[PosBuffer];
						PosBuffer++; // Пропускаем отступ
						cont.push_back(TokenSpace);
						LexToken Identifier = BufferPostLexerToken[PosBuffer];
						for (const auto& i : its->second)
							cont.push_back(i);
						cont.push_back(LexToken{ TTokenID::ScResOp, "::", 0, 0 });
						cont.push_back(Identifier);
						break;
					}
					default:
						cont.push_back(its->second[i]);
						break;
					}
				}
				cont.push_back(LexToken{ TTokenID::Asterisk, "*", 0, 0 });
				return cont;
			}
			default:
				return std::vector<LexToken>();
			}
			return std::vector<LexToken>();
		}
		return std::vector<LexToken>();
	}
	default:
		return std::vector<LexToken>();
	}
	return std::vector<LexToken>();
};

#endif // PRE_PARSER_HPP