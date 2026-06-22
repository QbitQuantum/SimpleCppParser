
#ifndef POST_LEXER_HPP
#define POST_LEXER_HPP
#pragma once

#include "TokenKinds.h"
#include "TokenDirectiveMap.hpp"

#include <vector>
#include <unordered_map>
#include <ctype.h>

class PostLexer {
private:
	using LexEnginePtr = Token(PostLexer::*)();
	int PosBuffer = 0;
	int SizeBufferBasic = 0;

	bool IsInclude = false;
private:
	std::vector<Token> LexerTokenBufferAdvance;
	std::vector<Token> LexerTokenBufferBasic;
	void Init();

	std::unordered_map<TokenKind, LexEnginePtr> map{ {
	{TokenKind::Quotation, &PostLexer::Quotation},   // "
	{TokenKind::Hash, &PostLexer::Hash},             // #
	{TokenKind::Ampersand, &PostLexer::Ampersand},   // &
	{TokenKind::Plus, &PostLexer::Plus},             // +
	{TokenKind::Minus, &PostLexer::Minus},           // -
	{TokenKind::Colon, &PostLexer::Colon},           // :
	{TokenKind::Less, &PostLexer::Less},             // <
	{TokenKind::Equals, &PostLexer::Equals},         // =
	{TokenKind::Greater, &PostLexer::Greater},       // >
	{TokenKind::Caret, &PostLexer::Caret},           // ^
	{TokenKind::Pipe, &PostLexer::Pipe},             // |
	{TokenKind::Asterisk, &PostLexer::Asterisk},     // *
	{TokenKind::Apostrophe, &PostLexer::Apostrophe}, // '
	{TokenKind::Literal, &PostLexer::Literal},       // Literal
	} };

	Token Quotation();
	Token Hash();
	Token Ampersand();
	Token Plus();
	Token Minus();
	Token Colon();
	Token Less();
	Token Equals();
	Token Greater();
	Token Caret();
	Token Pipe();
	Token Asterisk();
	Token Apostrophe();
	Token Literal();

	bool neof() {
		return PosBuffer < SizeBufferBasic;
	}

public:
	PostLexer(const std::vector<Token>& lexbuffer) {
		SizeBufferBasic = lexbuffer.size();
		LexerTokenBufferAdvance.reserve(SizeBufferBasic);
		LexerTokenBufferBasic.reserve(SizeBufferBasic);
		LexerTokenBufferBasic = lexbuffer;
		Init();
	}

	const std::vector<Token>& GetBufferPostLexerToken() const {
		return LexerTokenBufferAdvance;
	}
};

void PostLexer::Init() {

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
Token PostLexer::Quotation() /* " */ {

	IsInclude = false;

	Token TLexToken = {
		TokenKind::StringLiteral, "", 0, 0
	};

	std::string value = "";

	PosBuffer++;
	while (PosBuffer < SizeBufferBasic && LexerTokenBufferBasic[PosBuffer].type != TokenKind::Quotation) {
		value += LexerTokenBufferBasic[PosBuffer].value;
		PosBuffer++;
	};

	TLexToken.value = value;

	if (LexerTokenBufferAdvance.back().value == "l")
	{
		LexerTokenBufferAdvance.pop_back();
		TLexToken.type = TokenKind::WStringLiteral;
		return TLexToken;
	}
	return TLexToken;
}

// Обработка директив препроцессора
Token PostLexer::Hash() /* # */ {
	PosBuffer++;

	Token TLexToken = {
		TokenKind::Unknown, "", 0, 0
	};

	std::string directive = "#" + LexerTokenBufferBasic[PosBuffer].value;

	auto it = TokenDirectiveMap.find(directive);
	if (it != TokenDirectiveMap.end())
	{
		TLexToken.type = it->second;
		TLexToken.value = directive;
		IsInclude = TLexToken.type == TokenKind::IncludeDirective;
	}

	return TLexToken;
}

// Обработка амперсанда
Token PostLexer::Ampersand() /* & */ {
	Token TLexToken = {
		TokenKind::BitAnd, "&", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Ampersand)
	{
		TLexToken.type = TokenKind::And;
		TLexToken.value = "&&";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка плюса
Token PostLexer::Plus() /* + */ {
	Token TLexToken = {
		TokenKind::Plus, "+", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Plus)
	{
		TLexToken.type = TokenKind::Inc;
		TLexToken.value = "++";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка минуса
Token PostLexer::Minus() /* - */ {
	Token TLexToken = {
		TokenKind::Minus, "-", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Minus)
	{
		TLexToken.type = TokenKind::Dec;
		TLexToken.value = "--";
		PosBuffer++;
	}
	else if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Greater)
	{
		TLexToken.type = TokenKind::Arrow;
		TLexToken.value = "->";
		PosBuffer++;
	}
	return TLexToken;
}

// Обработка двоеточие
Token PostLexer::Colon() /* : */ {
	Token TLexToken = {
		TokenKind::Colon, ":", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Colon)
	{
		TLexToken.type = TokenKind::ScResOp;
		TLexToken.value = "::";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа уменьшения
Token PostLexer::Less() /* < */ {
	Token TLexToken = {
		TokenKind::Less, "<", 0, 0
	};

	PosBuffer++;

	if (IsInclude)
	{
		TLexToken.type = TokenKind::StringLiteral;
		std::string content = "";

		while (neof() && LexerTokenBufferBasic[PosBuffer].type != TokenKind::Greater) {
			content += LexerTokenBufferBasic[PosBuffer].value;
			PosBuffer++;
		}

		TLexToken.value = content;
		IsInclude = false;
		return TLexToken;
	}

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Equals)
	{
		TLexToken.type = TokenKind::LessEqual;
		TLexToken.value = "<=";
		PosBuffer++;
	}
	else if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Greater)
	{
		TLexToken.type = TokenKind::NotEqual;
		TLexToken.value = "<>";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа ровно
Token PostLexer::Equals() /* = */ {
	Token TLexToken = {
		TokenKind::Equals, "=", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Equals)
	{
		TLexToken.type = TokenKind::Equal;
		TLexToken.value = "==";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа больше
Token PostLexer::Greater() /* > */ {
	Token TLexToken = {
		TokenKind::Greater, ">", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Equals)
	{
		TLexToken.type = TokenKind::GreaterEqual;
		TLexToken.value = ">=";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа ^
Token PostLexer::Caret() /* ^ */ {
	return Token{
		TokenKind::Xor, "^", 0, 0
	};
}

// Обработка символа прямого слэша
Token PostLexer::Pipe() /* | */ {
	Token TLexToken = {
		TokenKind::BitOr, "|", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Pipe)
	{
		TLexToken.type = TokenKind::Or;
		TLexToken.value = "||";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка символа астерикса
Token PostLexer::Asterisk() /* * */ {
	Token TLexToken = {
		TokenKind::Asterisk, "*", 0, 0
	};

	if (PosBuffer + 1 < SizeBufferBasic &&
		LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Asterisk)
	{
		TLexToken.type = TokenKind::Power;
		TLexToken.value = "**";
		PosBuffer++;
	}

	return TLexToken;
}

// Обработка строковых литералов 
Token PostLexer::Apostrophe() /* ' */ {
	PosBuffer++;
	Token TLexToken = {
		TokenKind::CharLiteral, "", 0, 0
	};

	std::string value = "";

	while (PosBuffer < SizeBufferBasic && LexerTokenBufferBasic[PosBuffer].type != TokenKind::Apostrophe) {
		value += LexerTokenBufferBasic[PosBuffer].value;
		PosBuffer++;
	};

	TLexToken.value = value;

	if (LexerTokenBufferAdvance.back().value == "l")
	{
		LexerTokenBufferAdvance.pop_back();
		TLexToken.type = TokenKind::WCharLiteral;
		return TLexToken;
	}
	return TLexToken;
}

// Обработка типа литералов 
Token PostLexer::Literal() /* Literal */ {

	Token TLexToken = LexerTokenBufferBasic[PosBuffer];

	auto itKeywordMap = TokenKeywordMap.find(CppHash(TLexToken.value));
	if (itKeywordMap != TokenKeywordMap.end()) {
		TLexToken.type = itKeywordMap->second;
		return TLexToken;
	}

	// ctype.h [isdigit]
	if (!isdigit(TLexToken.value[0]))
	{
		TLexToken.type = TokenKind::IdentifierLiteral;
		return TLexToken;
	}

	if (TLexToken.value.length() > 1 && TLexToken.value[0] == '0') {
		switch (TLexToken.value[1])
		{
		case 'x':
		case 'X':
			TLexToken.type = TokenKind::HexLiteral;
			break;
		case 'b':
		case 'B':
			TLexToken.type = TokenKind::BinaryLiteral;
			break;
		default:
			// Восьмеричный литерал или просто 0
			TLexToken.type = TokenKind::IntegerLiteral;
			break;
		}
	}
	else
	{
		TLexToken.type = TokenKind::IntegerLiteral;

		bool IsFind = false;

		switch (TLexToken.value.back())
		{
		case 'f':
		case 'F':
			TLexToken.type = TokenKind::FloatLiteral;
			IsFind = true;
			break;
		case 'l':
		case 'L':
			TLexToken.type = TokenKind::LongDoubleLiteral;
			IsFind = true;
			break;
		default:
			break;
		}

		if (IsFind)
			TLexToken.value.erase(TLexToken.value.end() - 1);

		if ((PosBuffer + 1 < SizeBufferBasic) && LexerTokenBufferBasic[PosBuffer + 1].type == TokenKind::Dot)
		{
			PosBuffer++;
			TLexToken.type = TokenKind::DoubleLiteral;
			TLexToken.value += LexerTokenBufferBasic[PosBuffer].value;
		}

		switch (LexerTokenBufferAdvance.back().type)
		{
		case TokenKind::DoubleLiteral:
		case TokenKind::FloatLiteral:
		{
			auto Token = LexerTokenBufferAdvance.back();
			LexerTokenBufferAdvance.erase(LexerTokenBufferAdvance.end() - 1);
			TLexToken.value = Token.value + TLexToken.value;
			break;
		}
		default:
			break;
		}
	}

	if (LexerTokenBufferAdvance.back().type == TokenKind::Minus)
	{
		auto Token = LexerTokenBufferAdvance.back();
		LexerTokenBufferAdvance.erase(LexerTokenBufferAdvance.end() - 1);
		TLexToken.value = Token.value + TLexToken.value;
	}
	return TLexToken;
}
#endif // POST_LEXER_HPP