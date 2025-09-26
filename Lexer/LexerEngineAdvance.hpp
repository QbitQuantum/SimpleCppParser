
#include "TokenID.hpp"
#include "TokenDirectiveMap.hpp"

#include <vector>
#include <unordered_map>

class LexerEngineAdvance {
private:
	using LexEnginePtr = LexToken(LexerEngineAdvance::*)();
	int PosBuffer = 0;
	int SizeBufferBasic = 0;
private:
	std::vector<LexToken> LexerTokenBufferAdvance;
	std::vector<LexToken> LexerTokenBufferBasic;
	void Init(const std::vector<LexToken>& lexbuffer);

	std::unordered_map<TTokenID, LexEnginePtr> map{ {
	{TTokenID::Quotation, &LexerEngineAdvance::ProcessQuotation},   // "
	{TTokenID::Hash, &LexerEngineAdvance::ProcessHash},             // #
	{TTokenID::Dollar, &LexerEngineAdvance::ProcessDollar},         // $
	{TTokenID::Ampersand, &LexerEngineAdvance::ProcessAmpersand},   // &
	{TTokenID::Plus, &LexerEngineAdvance::ProcessPlus},             // +
	{TTokenID::Minus, &LexerEngineAdvance::ProcessMinus},           // -
	{TTokenID::Dot, &LexerEngineAdvance::ProcessDot},               // .
	{TTokenID::Colon, &LexerEngineAdvance::ProcessColon},           // :
	{TTokenID::Less, &LexerEngineAdvance::ProcessLess},             // <
	{TTokenID::Equals, &LexerEngineAdvance::ProcessEquals},         // =
	{TTokenID::Greater, &LexerEngineAdvance::ProcessGreater},       // >
	{TTokenID::At, &LexerEngineAdvance::ProcessAt},                 // @
	{TTokenID::Caret, &LexerEngineAdvance::ProcessCaret},           // ^
	{TTokenID::Underscore, &LexerEngineAdvance::ProcessUnderscore}, // _
	{TTokenID::Pipe, &LexerEngineAdvance::ProcessPipe},             // |
	{TTokenID::Asterisk, &LexerEngineAdvance::ProcessAsterisk},     // *
	{TTokenID::Apostrophe, &LexerEngineAdvance::ProcessApostrophe}  // '
	} };

	LexToken ProcessQuotation();
	LexToken ProcessHash();
	LexToken ProcessDollar();
	LexToken ProcessAmpersand();
	LexToken ProcessPlus();
	LexToken ProcessMinus();
	LexToken ProcessDot();
	LexToken ProcessColon();
	LexToken ProcessLess();
	LexToken ProcessEquals();
	LexToken ProcessGreater();
	LexToken ProcessAt();
	LexToken ProcessCaret();
	LexToken ProcessUnderscore();
	LexToken ProcessPipe();
	LexToken ProcessAsterisk();
	LexToken ProcessApostrophe();

	bool neof() {
		return PosBuffer < SizeBufferBasic;
	}

public:
	LexerEngineAdvance(const std::vector<LexToken> & lexbuffer) {
		SizeBufferBasic = lexbuffer.size();
		LexerTokenBufferAdvance.reserve(SizeBufferBasic);
		LexerTokenBufferBasic.reserve(SizeBufferBasic);
		LexerTokenBufferBasic = lexbuffer;
		Init(lexbuffer);
	}
};

void LexerEngineAdvance::Init(const std::vector<LexToken>& lexbuffer) {
	
	while (neof()) {
		auto it = map.find(lexbuffer[PosBuffer].type);

		LexerTokenBufferAdvance.push_back(
			it != map.end() ?
			(this->*it->second)() : lexbuffer[PosBuffer]
		);

		PosBuffer++;
	}
}

// Обработка строковых литералов 
LexToken LexerEngineAdvance::ProcessQuotation() /* " */ {
	PosBuffer++;
	LexToken TLexToken = {
		TTokenID::StringLiteral,
		"",
		0,
		0	
	};

	while (LexerTokenBufferBasic[PosBuffer].type != TTokenID::Quotation) {
		TLexToken.value += LexerTokenBufferBasic[PosBuffer].value;
		PosBuffer++;
	}
	PosBuffer++;
	return TLexToken;
}

// Обработка директив препроцессора
LexToken LexerEngineAdvance::ProcessHash() /* # */ {
	PosBuffer++;

	LexToken TLexToken = {
		TTokenID::Unknown,
		"",
		0,
		0
	};
	
	std::string directive = "#" + LexerTokenBufferBasic[PosBuffer].value;

	auto it = TokenDirectiveMap.find(directive);
	if (it != TokenDirectiveMap.end())
	{
		TLexToken.type = it->second;
		TLexToken.value = directive;
	}

	return TLexToken;
}

// Обработка имени идентификатора
LexToken LexerEngineAdvance::ProcessDollar() /* $ */ {
	PosBuffer++;

	LexToken TLexToken = {
		TTokenID::Identifier,
		"",
		0,
		0
	};

	std::string Identifier = "$" + LexerTokenBufferBasic[PosBuffer].value;

	TLexToken.value = Identifier;

	return TLexToken;
}