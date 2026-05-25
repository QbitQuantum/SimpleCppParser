
#ifndef PARSER_HPP
#define PARSER_HPP
#pragma once

#include "PostLexer.hpp"
#include "Node.hpp"

#include <vector>
#include <iostream>
#include <stdexcept>

class TokenStream {

public:
	std::vector<LexToken> Buffer;
	TokenStream() = default;
	
	size_t Pos = 0;

	void skipTrivia() {
		while (!eof() && (peek().type == TTokenID::Space || peek().type == TTokenID::LineFeed)) {
			++Pos;
		}
	}
	
	explicit TokenStream(const std::vector<LexToken>& buf) : Buffer(buf), Pos(0) {
		skipTrivia();
	}

	const LexToken& peek(size_t offset = 0) const {
		static LexToken eofToken{ TTokenID::neof, "", 0, 0 };
		size_t idx = Pos + offset;
		if (idx >= Buffer.size()) return eofToken;
		return Buffer[idx];
	}

	bool eof() const {
		return Pos >= Buffer.size();
	}

	bool match(TTokenID id) {
		if (peek().type == id) {
			++Pos;
			skipTrivia();
			return true;
		}
		return false;
	}

	const LexToken& consume(TTokenID id) {
		const LexToken& tok = peek();
		if (tok.type == id) {
			++Pos;
			skipTrivia();
			return tok;
		}
		// simple error recovery: return current token without advancing
		return tok;
	}
};

class Parser
{
private:
	TokenStream stream;
	std::vector<Node*> ast;

	Node* parseTopLevel();
	Node* parseAccess();
	Node* parseVar();
	Node* parseFunction();
	Node* parseClass();
	NodeBlock* parseBlock();

	NodeTypeQualifier* TypeQualifierParse();
public:
	std::vector<LexToken> ParserEngineBuffer;

	Parser(const PostLexer& advance) :
		ParserEngineBuffer(advance.GetBufferPostLexerToken()),
		stream(ParserEngineBuffer) {
	}

	Parser(const std::vector<LexToken>& Buffer) :
		ParserEngineBuffer(Buffer),
		stream(Buffer) {
		
	}

	void Parse() {
		
		while (!stream.eof()) {
			if (Node* node = parseTopLevel()) {
				ast.push_back(node);
			}
			else {
				// basic recovery: advance one token
				stream.consume(stream.peek().type);
			}
		}
	}

	const std::vector<Node*>& GetAst() const {
		return ast;
	}

	~Parser();

private:

};

Node* Parser::parseTopLevel() {
	switch (stream.peek().type) {
	case TTokenID::Access:   return parseAccess();
	case TTokenID::Var:      return parseVar();
	case TTokenID::Function: return parseFunction();
	case TTokenID::Class:    return parseClass();
	default:
		return nullptr;
	}
}

NodeTypeQualifier* Parser::TypeQualifierParse() {
	std::string Type = "";
	bool IsConst = false;
	bool IsRef = false;

	if (!stream.match(TTokenID::LeftBracket))
		return nullptr;
	
	while (!stream.match(TTokenID::RightBracket))
	{
		switch (stream.peek().type)
		{
		case TTokenID::Const: IsConst = true; stream.consume(TTokenID::Const);
			break;
		case TTokenID::Asterisk: IsRef = true; stream.consume(TTokenID::Asterisk);
			break;
		case TTokenID::ScResOp: Type = ""; stream.consume(TTokenID::ScResOp);
			break;
		case TTokenID::IdentifierLiteral:
			Type = stream.consume(TTokenID::IdentifierLiteral).value;
			break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
	}

	return new NodeTypeQualifier(new CType(Type, IsConst, IsRef));
};

Node* Parser::parseVar() {
	
	stream.consume(TTokenID::Var);

	auto TypeQualifier = TypeQualifierParse();

	if (!TypeQualifier)
		return nullptr;

	std::vector<NodeDeclaration*> ContainerDeclarationList;

	auto ParseInitializer = [&]() -> void {
		std::string Identifier;
		std::string Name;
		bool IsNamespaceToken = false;
		bool findEquals = false;

		// Парсим аргументы: var[const int] name = default
		while (true) {

			switch (stream.peek().type)
			{
			case TTokenID::Equals:
				stream.consume(TTokenID::Equals);
				findEquals = true;
				break;
			case TTokenID::ScResOp:
				stream.consume(TTokenID::ScResOp);
				Identifier = "";
				IsNamespaceToken = true;
				break;
			case TTokenID::IdentifierLiteral:
			{
				std::string TokenStr = stream.consume(TTokenID::IdentifierLiteral).value;
				if (!findEquals)
				{
					if (!Name.empty())
						throw std::runtime_error("not correct token Name");
					Name = TokenStr;
				}
				else
				{
					Identifier = TokenStr;
				}
				break;
			}
			case TTokenID::Comma:
			case TTokenID::Semicolon:
			{
				auto TokenType = stream.consume(stream.peek().type).type;
				NodeIdentifier* Initializer = Identifier.empty() ? nullptr : new NodeIdentifier(Identifier);

				ContainerDeclarationList.push_back(
					new NodeDeclaration(new NodeIdentifier(Name), Initializer)
				);

				Name = "";

				switch (TokenType)
				{
				case TTokenID::Comma:
					findEquals = false;
					break;
				case TTokenID::Semicolon:
					return;
				}
				break;
			}
			default:
				if (IsNamespaceToken)
					throw std::runtime_error("not correct token default");
				Identifier = stream.consume(stream.peek().type).value;
				break;
			}
		}
		};

	ParseInitializer();

	return new NodeDeclarationList(TypeQualifier, ContainerDeclarationList);
}

Node* Parser::parseFunction() {
	
	stream.consume(TTokenID::Function);
	
	size_t savedPos = stream.Pos;

	auto TypeQualifier = TypeQualifierParse();

	if (!TypeQualifier)
		return nullptr;

	if (!stream.match(TTokenID::LeftBracket)) {
		// Необязательный — если нет, пропускаем
	}
	else {
		while (!stream.match(TTokenID::RightBracket)) {
			stream.consume(stream.peek().type);
		}
	}
	
	// Имя функции
	if (stream.peek().type != TTokenID::IdentifierLiteral) {
		stream.Pos = savedPos;
		return nullptr;
	}
	std::string FunctionName = stream.consume(TTokenID::IdentifierLiteral).value;

	// Аргументы в скобках
	if (!stream.match(TTokenID::LeftParen)) {
		stream.Pos = savedPos;
		return nullptr;
	}

	std::vector<NodeDeclarationList*> ArgumentList;

	auto ParseInitializer = [&]() -> void {
		NodeTypeQualifier* ArgQualifier = nullptr;
		std::string Identifier;
		std::string ArgName;
		bool NewToken = true;
		bool IsNamespaceToken = false;
		bool findEquals = false;

		// Парсим аргументы: var[const int] name = default
		while (true) {
			
			switch (stream.peek().type)
			{
			case TTokenID::Var:
			{
				stream.consume(TTokenID::Var);
				if (!NewToken)
					throw std::runtime_error("stream.peek().type != TTokenID::Var");
				NewToken = false;
				ArgQualifier = TypeQualifierParse();
				break;
			}
			case TTokenID::Equals:
				stream.consume(TTokenID::Equals);
				findEquals = true;
				break;
			case TTokenID::ScResOp:
				stream.consume(TTokenID::ScResOp);
				Identifier = "";
				IsNamespaceToken = true;
				break;
			case TTokenID::IdentifierLiteral:
			{
				std::string TokenStr = stream.consume(TTokenID::IdentifierLiteral).value;
				if (!findEquals)
				{
					if (!ArgName.empty())
						throw std::runtime_error("not correct token ArgName");
					ArgName = TokenStr;
				}
				else
				{
					Identifier = TokenStr;
				}
				break;
			}
			case TTokenID::Comma:
			case TTokenID::RightParen:
			{
				auto TokenType = stream.consume(stream.peek().type).type;
				std::vector<NodeDeclaration*> Decls;
				if (!NewToken) {

					NodeIdentifier* nodeIdentifier = Identifier.empty() ? nullptr : new NodeIdentifier(Identifier);
					Decls.push_back(new NodeDeclaration(new NodeIdentifier(ArgName), nodeIdentifier));
					ArgumentList.push_back(new NodeDeclarationList(ArgQualifier, Decls));
				}
				ArgName = "";

				switch (TokenType)
				{
				case TTokenID::Comma:
					NewToken = true;
					findEquals = false;
					break;
				case TTokenID::RightParen:
					return;
				}
				break;
			}
			default:
				if (IsNamespaceToken)
					throw std::runtime_error("not correct token default");
				Identifier = stream.consume(stream.peek().type).value;
				break;
			}
		}
		};

	ParseInitializer();

	// Тело функции или ';'
	Node* body = nullptr;

	switch (stream.peek().type) {
	case TTokenID::LeftBrace:
		stream.consume(TTokenID::LeftBrace);
		body = parseBlock();
		break;
	case TTokenID::Semicolon:
		stream.consume(TTokenID::Semicolon);
		// Прототип функции
		break;
	default:
		throw std::runtime_error("not expected Semicolon or LeftBrace");
	}
	return new NodeFunction(TypeQualifier, FunctionName, ArgumentList, body);
}

Node* Parser::parseAccess() {
	while (!stream.match(TTokenID::Semicolon)) {
		stream.consume(stream.peek().type);
	}
	// Temporary stub
	return new NodeAccess();
};

Parser::~Parser()
{
	for (auto& i : ast)
		if (i) delete i;
}

NodeBlock* Parser::parseBlock() {
	if (!stream.match(TTokenID::LeftBrace))
		return nullptr;

	auto* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TTokenID::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TTokenID::Var:      stmt = parseVar(); break;
		case TTokenID::Function: stmt = parseFunction(); break;
		case TTokenID::Class:    stmt = parseClass(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}

	if (stream.peek().type == TTokenID::RightBrace)
		stream.consume(TTokenID::RightBrace);

	return block;
}

Node* Parser::parseClass() {
	// assume current token is Class
	stream.consume(TTokenID::Class);

	std::string name;
	std::string baseClass;

	// Parse class name
	if (stream.match(TTokenID::LeftBracket)) {
		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			name = stream.consume(TTokenID::IdentifierLiteral).value;
		}
		stream.match(TTokenID::RightBracket);
	}
	else if (stream.peek().type == TTokenID::IdentifierLiteral) {
		name = stream.consume(TTokenID::IdentifierLiteral).value;
	}

	// Провераяем наличие базового класса
	if (stream.match(TTokenID::Colon)) {
		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			baseClass = stream.consume(TTokenID::IdentifierLiteral).value;
		}
	}

	NodeBlock* body = nullptr;
	if (stream.peek().type == TTokenID::LeftBrace) {
		body = parseBlock();
	}

	return new NodeClass(name, baseClass, body);
}

#endif // PARSER_HPP