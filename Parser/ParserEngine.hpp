
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
	{TTokenID::IncludeDeirective, &ParserEngine::ProcessIncludeDeirective},
	{TTokenID::Using, &ParserEngine::ProcessUsing},
	{TTokenID::Class, &ParserEngine::ProcessClass},
	{TTokenID::Function, &ParserEngine::ProcessFunction},
	{TTokenID::Var, &ParserEngine::ProcessVar},
	}};

	void ProcessIncludeDeirective() {};
	void ProcessUsing() {};
	void ProcessClass() {};
	void ProcessFunction() {};
	void ProcessVar() {};

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