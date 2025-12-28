#ifndef PRE_PARSER_HPP
#define PRE_PARSER_HPP
#pragma once

#include "TokenID.hpp"

#include <vector>

class PreParser {
private:
	using LexEnginePtr = std::vector<LexToken>(PreParser::*)();
	int PosBuffer = 0;
	int SizeBufferBasic = 0;
	
private:
	std::vector<LexToken> BufferPreParserToken;
	std::vector<LexToken> BufferPostLexerToken;
	void Init();

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
		BufferPreParserToken.push_back(BufferPostLexerToken[PosBuffer]);
		PosBuffer++;
	}
};

#endif // PRE_PARSER_HPP