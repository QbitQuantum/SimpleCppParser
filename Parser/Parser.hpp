
#ifndef PARSER_HPP
#define PARSER_HPP
#pragma once

#include "PostLexer.hpp"
#include "Node.hpp"

class Parser
{
	using ParserEnginePtr = void(Parser::*)();
private:
	int PosBuffer = 0;
	int SizeBufferParser = 0;

	void Init();
	bool neof() {
		return PosBuffer < SizeBufferParser;
	}

	std::unordered_map<TTokenID, ParserEnginePtr> map{{
	{TTokenID::Var, &Parser::Var},
	}};

	void Var() {};

public:
	std::vector<LexToken> ParserEngineBuffer;

	Parser(const PostLexer& advance) : 
		ParserEngineBuffer(advance.GetBufferPostLexerToken()) {
		Init();
	};

	Parser(const std::vector<LexToken>& Buffer) : 
		ParserEngineBuffer(Buffer) {
		Init();
	};

	~Parser();

private:

};

void Parser::Init() {
	SizeBufferParser = ParserEngineBuffer.size();
	
	while (neof()) {
		if (auto it = map.find(ParserEngineBuffer[PosBuffer].type); 
		it != map.end()) (this->*it->second)();
		PosBuffer++;
	}
};

Parser::~Parser()
{
}

#endif // PARSER_HPP