
#ifndef PARSER_ENGINE_HPP
#define PARSER_ENGINE_HPP
#pragma once

#include "LexerEngineAdvance.hpp"

class ParserEngine
{
public:
	std::vector<LexToken> ParserEngineBuffer;

	ParserEngine(const LexerEngineAdvance& advance) : 
		ParserEngineBuffer(advance.GetBufferLexerAdvanceToken()) {};

	ParserEngine(const std::vector<LexToken>& Buffer) : 
		ParserEngineBuffer(Buffer) {};

	~ParserEngine();

private:

};

ParserEngine::~ParserEngine()
{
}

#endif // PARSER_ENGINE_HPP