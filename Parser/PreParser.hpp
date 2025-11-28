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

	std::unordered_map<TTokenID, LexEnginePtr> map{ {
	{TTokenID::Using,				&PreParser::Using},   // Using
	} };

	std::vector<LexToken> Using();

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

#endif // PRE_PARSER_HPP