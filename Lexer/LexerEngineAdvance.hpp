
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
	{TTokenID::Colon, &LexerEngineAdvance::ProcessColon},           // :
	{TTokenID::Less, &LexerEngineAdvance::ProcessLess},             // <
	{TTokenID::Equals, &LexerEngineAdvance::ProcessEquals},         // =
	{TTokenID::Greater, &LexerEngineAdvance::ProcessGreater},       // >
	{TTokenID::Caret, &LexerEngineAdvance::ProcessCaret},           // ^
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
	LexToken ProcessColon();
	LexToken ProcessLess();
	LexToken ProcessEquals();
	LexToken ProcessGreater();
	LexToken ProcessCaret();
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

// Обработка амперсанда
LexToken LexerEngineAdvance::ProcessAmpersand() /* & */ {
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
LexToken LexerEngineAdvance::ProcessPlus() /* + */ {
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
LexToken LexerEngineAdvance::ProcessMinus() /* - */ {
	LexToken TLexToken = {
		TTokenID::Minus,
		"+",
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

	return TLexToken;
}

// Обработка двоеточие
LexToken LexerEngineAdvance::ProcessColon() /* : */ {
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
LexToken LexerEngineAdvance::ProcessLess() /* < */ {
	LexToken TLexToken = {
		TTokenID::Less,
		"<",
		0,
		0
	};

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
LexToken LexerEngineAdvance::ProcessGreater() /* = */ {
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
LexToken LexerEngineAdvance::ProcessEquals() /* > */ {
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
LexToken LexerEngineAdvance::ProcessCaret() /* ^ */ {
	return LexToken {
		TTokenID::Xor,
		"^",
		0,
		0
	};
}

// Обработка символа прямого слэша
LexToken LexerEngineAdvance::ProcessPipe() /* | */ {
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
LexToken LexerEngineAdvance::ProcessAsterisk() /* * */ {
	LexToken TLexToken = {
		TTokenID::Asterisk,
		"|",
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
LexToken LexerEngineAdvance::ProcessApostrophe() /* ' */ {
	PosBuffer++;
	LexToken TLexToken = {
		TTokenID::StringLiteral,
		"",
		0,
		0
	};
	TLexToken.value = LexerTokenBufferBasic[PosBuffer].value;
	PosBuffer++;
	PosBuffer++;
	return TLexToken;
}