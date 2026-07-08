
#ifndef POST_LEXER_HPP
#define POST_LEXER_HPP
#pragma once

#include "TokenKinds.h"
#include "TokenDirectiveMap.hpp"

#include <vector>
#include <unordered_map>
#include <ctype.h>
#include <iostream>
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
Token PostLexer::Literal() {
	Token TLexToken = LexerTokenBufferBasic[PosBuffer];

	// Проверяем ключевые слова
	auto itKeywordMap = TokenKeywordMap.find(CppHash(TLexToken.value));
	if (itKeywordMap != TokenKeywordMap.end()) {
		TLexToken.type = itKeywordMap->second;
		return TLexToken;
	}

	// Определяем тип числового литерала
	const std::string& value = TLexToken.value;
	size_t len = value.length();

	if (len == 0) return TLexToken;

	// Идентификаторы не могут начинаться с .
	// Только числа
	if (value[0] != '.' && !isxdigit(value[0]))
	{
		TLexToken.type = TokenKind::IdentifierLiteral;
		return TLexToken;
	}

	// Шестнадцатеричные: 0x...
	if (len > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
		TLexToken.type = TokenKind::HexLiteral;
		return TLexToken;
	}

	// Двоичные: 0b...
	if (len > 2 && value[0] == '0' && (value[1] == 'b' || value[1] == 'B')) {
		TLexToken.type = TokenKind::BinaryLiteral;
		return TLexToken;
	}

	// Восьмеричные: 0...
	if (len > 1 && value[0] == '0') {
		TLexToken.type = TokenKind::IntegerLiteral;
		return TLexToken;
	}

	// Проверяем, есть ли в числе суффиксы типов
	char lastChar = value.back();
	if (lastChar == 'f' || lastChar == 'F') {
		TLexToken.type = TokenKind::FloatLiteral;
		TLexToken.value = value.substr(0, len - 1);
		return TLexToken;
	}

	if (lastChar == 'l' || lastChar == 'L') {
		TLexToken.type = TokenKind::LongDoubleLiteral;
		TLexToken.value = value.substr(0, len - 1);
		return TLexToken;
	}

	// Проверяем, есть ли точка (числа с плавающей точкой)
	if (value.find('.') != std::string::npos ||
		value.find('e') != std::string::npos ||
		value.find('E') != std::string::npos ||
		value.find('p') != std::string::npos ||
		value.find('P') != std::string::npos) {
		TLexToken.type = TokenKind::DoubleLiteral;
		return TLexToken;
	}

	// Обычное целое число
	TLexToken.type = TokenKind::IntegerLiteral;
	return TLexToken;
}

#endif // POST_LEXER_HPP