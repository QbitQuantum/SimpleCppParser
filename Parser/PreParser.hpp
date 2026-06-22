#ifndef PRE_PARSER_HPP
#define PRE_PARSER_HPP
#pragma once

#include "TokenKinds.h"

#include <vector>

class PreParser {
private:
	using LexEnginePtr = std::vector<Token>(PreParser::*)();
	int PosBuffer = 0;
	int SizeBufferBasic = 0;
	
private:
	std::vector<Token> BufferPreParserToken;
	std::vector<Token> BufferPostLexerToken;
	void Init();

	bool neof() {
		return PosBuffer < SizeBufferBasic;
	}

public:
	PreParser(const std::vector<Token>& lexbuffer) {
		SizeBufferBasic = lexbuffer.size();
		BufferPreParserToken.reserve(SizeBufferBasic);
		BufferPostLexerToken.reserve(SizeBufferBasic);
		BufferPostLexerToken = lexbuffer;
		Init();
	}

	const std::vector<Token>& GetBufferPreParserToken() const {
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