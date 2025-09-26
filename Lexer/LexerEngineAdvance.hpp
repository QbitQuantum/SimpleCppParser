
#include "TokenID.hpp"

#include <vector>
#include <unordered_map>

class LexerEngineAdvance {
private:
	using LexEnginePtr = LexToken(LexerEngineAdvance::*)();
private:
	std::vector<LexToken> BufferToken;
	void Init(const std::vector<LexToken>& lexbuffer);

	std::unordered_map<TTokenID, LexEnginePtr> map{ {
	{TTokenID::Quotation, &LexerEngineAdvance::ProcessQuotation},   // "
	{TTokenID::Hash, &LexerEngineAdvance::ProcessHash},             // #
	{TTokenID::Dollar, &LexerEngineAdvance::ProcessDollar},         // $
	{TTokenID::Percent, &LexerEngineAdvance::ProcessPercent},       // %
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
	LexToken ProcessPercent();
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

public:
	LexerEngineAdvance(const std::vector<LexToken> & lexbuffer) {
		Init(lexbuffer);
	}
};

void LexerEngineAdvance::Init(const std::vector<LexToken>& lexbuffer) {
	for (const LexToken& TokenID : lexbuffer)
	{
		if (auto it = map.find(TokenID.type); it != map.end()) {
			BufferToken.push_back((this->*it->second)());
		}
		else
		{
			BufferToken.push_back(TokenID);
		}
	}
}