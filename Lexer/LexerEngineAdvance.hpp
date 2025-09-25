
#include "TokenID.hpp"

#include <vector>
#include <unordered_map>

class LexerEngineAdvance {
private:
	using LexEnginePtr = void (LexerEngineAdvance::*)();
private:
	std::vector<LexToken> BufferToken;
	void Init(const std::vector<LexToken>& lexbuffer);

	std::unordered_map<TTokenID, LexEnginePtr> map{ {
	{TTokenID::Quotation, &LexerEngineAdvance::ProcessQuotation},   // "
	{TTokenID::Hash, &LexerEngineAdvance::ProcessHash},             // #
	{TTokenID::Dollar, &LexerEngineAdvance::ProcessDollar},         // $
	{TTokenID::Percent, &LexerEngineAdvance::ProcessPercent},       // %
	{TTokenID::Ampersand, &LexerEngineAdvance::ProcessAmpersand},   // &
	{TTokenID::LeftParen, &LexerEngineAdvance::ProcessLeftParen},   // (
	{TTokenID::RightParen, &LexerEngineAdvance::ProcessRightParen}, // )
	{TTokenID::LeftBrace, &LexerEngineAdvance::ProcessLeftBrace},   // {
	{TTokenID::RightBrace, &LexerEngineAdvance::ProcessRightBrace}, // }
	{TTokenID::LeftBracket, &LexerEngineAdvance::ProcessLeftBracket}, // [
	{TTokenID::RightBracket, &LexerEngineAdvance::ProcessRightBracket}, // ]
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

	void ProcessQuotation();
	void ProcessHash();
	void ProcessDollar();
	void ProcessPercent();
	void ProcessAmpersand();
	void ProcessLeftParen();
	void ProcessRightParen();
	void ProcessLeftBrace();
	void ProcessRightBrace();
	void ProcessLeftBracket();
	void ProcessRightBracket();
	void ProcessPlus();
	void ProcessMinus();
	void ProcessDot();
	void ProcessColon();
	void ProcessLess();
	void ProcessEquals();
	void ProcessGreater();
	void ProcessAt();
	void ProcessCaret();
	void ProcessUnderscore();
	void ProcessPipe();
	void ProcessAsterisk();
	void ProcessApostrophe();

public:
	LexerEngineAdvance(const std::vector<LexToken> & lexbuffer) {
		Init(lexbuffer);
	}
};

void LexerEngineAdvance::Init(const std::vector<LexToken>& lexbuffer) {

}