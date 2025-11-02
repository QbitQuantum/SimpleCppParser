
#ifndef LEXER_ENGINE_ADVANCE_HPP
#define LEXER_ENGINE_ADVANCE_HPP
#pragma once

#include "TokenID.hpp"
#include "TokenDirectiveMap.hpp"

#include <vector>
#include <unordered_map>
#include <ctype.h>

class LexerEngineAdvance {
private:
	using LexEnginePtr = LexToken(LexerEngineAdvance::*)();
	int PosBuffer = 0;
	int SizeBufferBasic = 0;

	bool IsInclude = false;
private:
	std::vector<LexToken> LexerTokenBufferAdvance;
	std::vector<LexToken> LexerTokenBufferBasic;
	void Init();

	std::unordered_map<TTokenID, LexEnginePtr> map{ {
	{TTokenID::Quotation, &LexerEngineAdvance::Quotation},   // "
	{TTokenID::Hash, &LexerEngineAdvance::Hash},             // #
	{TTokenID::Ampersand, &LexerEngineAdvance::Ampersand},   // &
	{TTokenID::Plus, &LexerEngineAdvance::Plus},             // +
	{TTokenID::Minus, &LexerEngineAdvance::Minus},           // -
	{TTokenID::Colon, &LexerEngineAdvance::Colon},           // :
	{TTokenID::Less, &LexerEngineAdvance::Less},             // <
	{TTokenID::Equals, &LexerEngineAdvance::Equals},         // =
	{TTokenID::Greater, &LexerEngineAdvance::Greater},       // >
	{TTokenID::Caret, &LexerEngineAdvance::Caret},           // ^
	{TTokenID::Pipe, &LexerEngineAdvance::Pipe},             // |
	{TTokenID::Asterisk, &LexerEngineAdvance::Asterisk},     // *
	{TTokenID::Apostrophe, &LexerEngineAdvance::Apostrophe}, // '
	{TTokenID::Literal, &LexerEngineAdvance::Literal},       // Literal
	} };

	LexToken Quotation();
	LexToken Hash();
	LexToken Ampersand();
	LexToken Plus();
	LexToken Minus();
	LexToken Colon();
	LexToken Less();
	LexToken Equals();
	LexToken Greater();
	LexToken Caret();
	LexToken Pipe();
	LexToken Asterisk();
	LexToken Apostrophe();
	LexToken Literal();

	bool neof() {
		return PosBuffer < SizeBufferBasic;
	}

public:
	LexerEngineAdvance(const std::vector<LexToken>& lexbuffer) {
		SizeBufferBasic = lexbuffer.size();
		LexerTokenBufferAdvance.reserve(SizeBufferBasic);
		LexerTokenBufferBasic.reserve(SizeBufferBasic);
		LexerTokenBufferBasic = lexbuffer;
		Init();
	}

	const std::vector<LexToken>& GetBufferLexerAdvanceToken() const {
		return LexerTokenBufferAdvance;
	}
};

void LexerEngineAdvance::Init() {

	while (neof()) {
		auto it = map.find(LexerTokenBufferBasic[PosBuffer].type);

		LexerTokenBufferAdvance.push_back(
			it != map.end() ?
			(this->*it->second)() : LexerTokenBufferBasic[PosBuffer]
		);

		PosBuffer++;
	}
}

// Обработка строковых литералов 
LexToken LexerEngineAdvance::Quotation() /* " */ {

	IsInclude = false;

	LexToken TLexToken = {
		TTokenID::StringLiteral,
		"",
		0,
		0
	};

	std::string value = "";

	PosBuffer++;
	while (PosBuffer < SizeBufferBasic && LexerTokenBufferBasic[PosBuffer].type != TTokenID::Quotation) {
		value += LexerTokenBufferBasic[PosBuffer].value;
		PosBuffer++;
	};

	TLexToken.value = value;

	if (LexerTokenBufferAdvance.back().value == "l")
	{
		LexerTokenBufferAdvance.pop_back();
		TLexToken.type = TTokenID::WStringLiteral;
		return TLexToken;
	}
	return TLexToken;
}

// Обработка директив препроцессора
LexToken LexerEngineAdvance::Hash() /* # */ {
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
		IsInclude = TLexToken.type == TTokenID::IncludeDirective;
	}

	return TLexToken;
}

// Обработка амперсанда
LexToken LexerEngineAdvance::Ampersand() /* & */ {
	LexToken TLexToken = {
		TTokenID::BitAnd,
		"&",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Ampersand)
	{
		TLexToken.type = TTokenID::And;
		TLexToken.value = "&&";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка плюса
LexToken LexerEngineAdvance::Plus() /* + */ {
	LexToken TLexToken = {
		TTokenID::Plus,
		"+",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Plus)
	{
		TLexToken.type = TTokenID::Inc;
		TLexToken.value = "++";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка минуса
LexToken LexerEngineAdvance::Minus() /* - */ {
	LexToken TLexToken = {
		TTokenID::Minus,
		"-",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Minus)
	{
		TLexToken.type = TTokenID::Dec;
		TLexToken.value = "--";
		PosBuffer++;
	}
	else if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Greater)
	{
		TLexToken.type = TTokenID::Pointer;
		TLexToken.value = "->";
		PosBuffer++;
	}
	return TLexToken;
}

// Обработка двоеточие
LexToken LexerEngineAdvance::Colon() /* : */ {
	LexToken TLexToken = {
		TTokenID::Colon,
		":",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Colon)
	{
		TLexToken.type = TTokenID::ScResOp;
		TLexToken.value = "::";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа уменьшения
LexToken LexerEngineAdvance::Less() /* < */ {
	LexToken TLexToken = {
		TTokenID::Less,
		"<",
		0,
		0
	};

	PosBuffer++;

	if (IsInclude)
	{
		TLexToken.type = TTokenID::StringLiteral;
		std::string content = "";

		while (neof() && LexerTokenBufferBasic[PosBuffer].type != TTokenID::Greater) {
			content += LexerTokenBufferBasic[PosBuffer].value;
			PosBuffer++;
		}

		TLexToken.value = content;
		IsInclude = false;
		return TLexToken;
	}

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Equals)
	{
		TLexToken.type = TTokenID::LessEqual;
		TLexToken.value = "<=";
		PosBuffer++;
	}
	else if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Greater)
	{
		TLexToken.type = TTokenID::NotEqual;
		TLexToken.value = "<>";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа ровно
LexToken LexerEngineAdvance::Equals() /* = */ {
	LexToken TLexToken = {
		TTokenID::Equals,
		"=",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Equals)
	{
		TLexToken.type = TTokenID::Equal;
		TLexToken.value = "==";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа больше
LexToken LexerEngineAdvance::Greater() /* > */ {
	LexToken TLexToken = {
		TTokenID::Greater,
		">",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Equals)
	{
		TLexToken.type = TTokenID::GreaterEqual;
		TLexToken.value = ">=";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа ^
LexToken LexerEngineAdvance::Caret() /* ^ */ {
	return LexToken{
		TTokenID::Xor,
		"^",
		0,
		0
	};
}

// Обработка символа прямого слэша
LexToken LexerEngineAdvance::Pipe() /* | */ {
	LexToken TLexToken = {
		TTokenID::BitOr,
		"|",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Pipe)
	{
		TLexToken.type = TTokenID::Or;
		TLexToken.value = "||";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа астерикса
LexToken LexerEngineAdvance::Asterisk() /* * */ {
	LexToken TLexToken = {
		TTokenID::Asterisk,
		"*",
		0,
		0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Asterisk)
	{
		TLexToken.type = TTokenID::Power;
		TLexToken.value = "**";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка строковых литералов 
LexToken LexerEngineAdvance::Apostrophe() /* ' */ {
	PosBuffer++;
	LexToken TLexToken = {
		TTokenID::CharLiteral,
		"",
		0,
		0
	};

	std::string value = "";

	while (PosBuffer < SizeBufferBasic && LexerTokenBufferBasic[PosBuffer].type != TTokenID::Apostrophe) {
		value += LexerTokenBufferBasic[PosBuffer].value;
		PosBuffer++;
	};

	TLexToken.value = value;

	if (LexerTokenBufferAdvance.back().value == "l")
	{
		LexerTokenBufferAdvance.pop_back();
		TLexToken.type = TTokenID::WCharLiteral;
		return TLexToken;
	}
	return TLexToken;
}

// Обработка типа литералов 
LexToken LexerEngineAdvance::Literal() /* Literal */ {

	LexToken TLexToken = LexerTokenBufferBasic[PosBuffer];
	TLexToken.type = TTokenID::IdentifierLiteral;

	// ctype.h [isdigit]
	if (!isdigit(TLexToken.value[0]))
		return TLexToken;

	if (TLexToken.value != "0" && TLexToken.value[0] == '0') {
		switch (TLexToken.value[1])
		{
		case 'x':
		case 'X':
			TLexToken.type = TTokenID::HexLiteral;
			break;
		case 'b':
		case 'B':
			TLexToken.type = TTokenID::BinaryLiteral;
			break;
		default:
			break;
		}
	}
	else
	{
		if ((PosBuffer + 1 < SizeBufferBasic) &&
			LexerTokenBufferBasic[PosBuffer + 1].type == TTokenID::Dot)
		{
			PosBuffer++;
			TLexToken.type = TTokenID::FloatLiteral;

			if (PosBuffer + 1 < SizeBufferBasic &&
				isdigit(LexerTokenBufferBasic[PosBuffer + 1].value[0]))
			{
				PosBuffer++;
				TLexToken.value += "." + LexerTokenBufferBasic[PosBuffer].value;
			}
		}
		else
		{
			TLexToken.type = TTokenID::IntegerLiteral;
		}
	}

	if (TLexToken.type != TTokenID::IdentifierLiteral)
	{
		if (LexerTokenBufferAdvance.back().type == TTokenID::Minus)
		{
			LexerTokenBufferAdvance.erase(LexerTokenBufferAdvance.end() - 1);
			TLexToken.value = "-" + TLexToken.value;
		}
		return TLexToken;
	}

	return TLexToken;
}
#endif // LEXER_ENGINE_ADVANCE_HPP