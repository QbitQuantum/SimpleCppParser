
#ifndef PARSER_ENGINE_HPP
#define PARSER_ENGINE_HPP
#pragma once

#include "LexerEngineAdvance.hpp"
#include "Node.hpp"

class ParserEngine
{
	using ParserEnginePtr = void(ParserEngine::*)();
private:
	int PosBuffer = 0;
	int SizeBufferParser = 0;

	void Init();
	bool neof() {
		return PosBuffer < SizeBufferParser;
	}

	std::unordered_map<TTokenID, ParserEnginePtr> map{{
	{TTokenID::IncludeDirective, &ParserEngine::IncludeDirective},
	{TTokenID::Using, &ParserEngine::Using},
	{TTokenID::Class, &ParserEngine::Class},
	{TTokenID::Function, &ParserEngine::Function},
	{TTokenID::Var, &ParserEngine::Var},
	}};

	void IncludeDirective() {};
	void Using() {};
	void Class() {};
	void Function() {};
	void Var() {};

public:
	std::vector<LexToken> ParserEngineBuffer;

	ParserEngine(const LexerEngineAdvance& advance) : 
		ParserEngineBuffer(advance.GetBufferLexerAdvanceToken()) {
		Init();
	};

	ParserEngine(const std::vector<LexToken>& Buffer) : 
		ParserEngineBuffer(Buffer) {
		Init();
	};

	~ParserEngine();

private:

};

void ParserEngine::Init() {
	SizeBufferParser = ParserEngineBuffer.size();
	
	while (neof()) {
		if (auto it = map.find(ParserEngineBuffer[PosBuffer].type); 
		it != map.end()) (this->*it->second)();
		PosBuffer++;
	}
};

ParserEngine::~ParserEngine()
{
}

#endif // PARSER_ENGINE_HPP