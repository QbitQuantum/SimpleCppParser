
#ifndef PARSER_HPP
#define PARSER_HPP
#pragma once

#include "PostLexer.hpp"
#include "Node.hpp"

#include <vector>
#include <iostream>
#include <stdexcept>

class TokenStream
{

	std::vector<Token> Buffer;
	size_t Pos = 0;

public:

	TokenStream() = default;

	explicit TokenStream(const std::vector<Token>& buf) : Buffer(buf), Pos(0) {
		skipTrivia();
	}

	void skipTrivia() {
		while (!eof() && (peek().type == TokenKind::Space || peek().type == TokenKind::LineFeed)) {
			++Pos;
		}
	}

	const Token& peek(size_t offset = 0) const {
		static Token eofToken{ TokenKind::neof, "", 0, 0 };
		size_t idx = Pos + offset;
		if (idx >= Buffer.size()) return eofToken;
		return Buffer[idx];
	}

	bool eof() const {
		return Pos >= Buffer.size();
	}

	bool match(TokenKind id) {
		if (peek().type == id) {
			++Pos;
			skipTrivia();
			return true;
		}
		return false;
	}

	const Token& consume(TokenKind id) {
		const Token& tok = peek();
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
	Node* parseClassBlock();
	Node* parseStructBlock();
	Node* parseFunctionBlock();
	Node* parseNamespaceBlock();
	Node* parseWhileBlock();
	Node* parseTryBlock();
	Node* parseCatchBlock();

	Node* parseAccess();
	Node* parseUsing();
	Node* parseVar();
	Node* parseIdentifier();
	Node* parseFunction();
	Node* parseLambda();
	Node* parseWhile();
	Node* parseGenericParametrs();
	Node* parseGenericParametrsConcretic();
	Node* parseClass();
	Node* parseStruct();
	Node* parseConstructor();
	Node* parseDestructor();
	Node* parseExpression();
	Node* parseDeclaration();
	Node* parseProperty();
	Node* parseNew();
	Node* parseDelete();
	Node* parseNullptr();
	Node* parseNodeCall(std::string Func);
	Node* parseNamespace();
	Node* parseTryCatch();

	Node* parseNodeInteger();
	Node* parseNodeFloating();
	Node* parseNodeBoolean();
	Node* parseNodeString();
	Node* parseNodeCharacter();

	std::vector<Node*> parseArgumentList();
	Node* parseArgument();

	Node* parseType();
	std::string parse_namespace();
public:
	std::vector<Token> ParserEngineBuffer;

	Parser(const PostLexer& advance) :
		ParserEngineBuffer(advance.GetBufferPostLexerToken()),
		stream(ParserEngineBuffer) {
	}

	Parser(const std::vector<Token>& Buffer) :
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
	case TokenKind::Access:   return parseAccess();
	case TokenKind::Using:    return parseUsing();
	case TokenKind::Var:      return parseVar();
	case TokenKind::Function: return parseFunction();
	case TokenKind::Class:    return parseClass();
	case TokenKind::IdentifierLiteral: return parseIdentifier();
	case TokenKind::Namespace: return parseNamespace();
	default:
		return nullptr;
	}
}

Node* Parser::parseNamespace() {

	stream.consume(TokenKind::Namespace);
	
	// Stub
	if (stream.match(TokenKind::LeftBracket))
		while (stream.peek().type != TokenKind::RightBracket)
			stream.consume(stream.peek().type);
	stream.consume(TokenKind::RightBracket);

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	std::string Name = parse_namespace();

	if (stream.peek().type != TokenKind::LeftBrace)
		throw std::runtime_error("Expected LeftBrace token");
	stream.consume(TokenKind::LeftBrace);

	Node* body = parseNamespaceBlock();

	if (stream.peek().type == TokenKind::RightBrace)
		stream.consume(TokenKind::RightBrace);

	return new NodeNamespace(Name, body);
}

Node* Parser::parseNamespaceBlock() {

	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:		stmt = parseVar(); break;
		case TokenKind::Function:	stmt = parseFunction(); break;
		case TokenKind::Class:		stmt = parseClass(); break;
		case TokenKind::Namespace:	stmt = parseNamespace(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}
	return block;
}

Node* Parser::parseProperty() {
	stream.consume(TokenKind::Property); // __property
	
	// тип (int)
	Node* Type = parseType();
	if (!Type) throw std::runtime_error("Expected type in __property");

	// имя (Value)
	std::string name = stream.consume(TokenKind::IdentifierLiteral).value;
	
	// {
	if (!stream.match(TokenKind::LeftBrace))
		throw std::runtime_error("Expected '{' in __property");

	std::string getter, setter;
	while (!stream.match(TokenKind::RightBrace)) {
		if (stream.match(TokenKind::Read)) {
			if (!stream.match(TokenKind::Equals))
				throw std::runtime_error("Expected '=' after 'read'");
			getter = parse_namespace(); // __getValue
		}
		else if (stream.match(TokenKind::Write)) {
			if (!stream.match(TokenKind::Equals))
				throw std::runtime_error("Expected '=' after 'write'");
			setter = parse_namespace(); // __setValue
		}
		else {
			stream.consume(stream.peek().type);
		}
	}
	
	return new NodeProperty(name, Type, getter, setter);
}

std::string Parser::parse_namespace() {
	std::string Identifier = "";
	while (true) {
		switch (stream.peek().type) {
		case TokenKind::IdentifierLiteral:
			Identifier = stream.consume(TokenKind::IdentifierLiteral).value;
			break;
		case TokenKind::ScResOp:
			stream.consume(TokenKind::ScResOp);
			if (stream.peek().type != TokenKind::IdentifierLiteral)
				throw std::runtime_error("Expected identifier after '::'");
			Identifier = "";
			break;
		default:
			return Identifier;
		}
	}
}

Node* Parser::parseType() {
	std::string Type = "";
	bool IsConst = false;
	bool IsRef = false;

	if (!stream.match(TokenKind::LeftBracket))
		return nullptr;

	while (!stream.match(TokenKind::RightBracket))
	{
		switch (stream.peek().type)
		{
		case TokenKind::Const: IsConst = true; stream.consume(TokenKind::Const);
			break;
		case TokenKind::Asterisk: IsRef = true; stream.consume(TokenKind::Asterisk);
			break;
		case TokenKind::IdentifierLiteral:
			Type = parse_namespace();
			break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
	}

	return new NodeType(Type, IsConst, IsRef);
};

Node* Parser::parseVar() {

	stream.consume(TokenKind::Var);

	auto Type = parseType();

	if (!Type)
		return nullptr;

	std::vector<Node*> ContainerDeclarationList;

	ContainerDeclarationList.push_back(parseDeclaration());

	// Парсим аргументы: name = default
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		ContainerDeclarationList.push_back(parseDeclaration());
	}

	stream.consume(TokenKind::Semicolon);

	return new NodeDeclarationList(Type, ContainerDeclarationList);
}

Node* Parser::parseDeclaration() {

	NodeIdentifier* Identifier = nullptr;
	Node* Exptression = nullptr;

	// Имя может быть пустое. По хорошему исключить такую фигню
	if (stream.peek().type == TokenKind::IdentifierLiteral)
		// То что может быть Namespace::Name в имене идентикатора - работа семантера
		Identifier = new NodeIdentifier(parse_namespace());

	if (stream.peek().type == TokenKind::Equals)
	{
		if (!Identifier)
			throw std::runtime_error("Expected identifier");
		stream.consume(TokenKind::Equals);
		Exptression = parseExpression();
	}

	return new NodeDeclaration(Identifier, Exptression);
}

Node* Parser::parseExpression() {
	switch (stream.peek().type)
	{
	case TokenKind::New:
		return parseNew();
	case TokenKind::Delete_:
		return parseDelete();
	case TokenKind::NullptrLiteral:
		return parseNullptr();
	case TokenKind::IdentifierLiteral:
		return parseIdentifier();
	case TokenKind::IntegerLiteral:
	case TokenKind::HexLiteral:
	case TokenKind::BinaryLiteral:
		return parseNodeInteger();
	case TokenKind::FloatLiteral:
	case TokenKind::DoubleLiteral:
	case TokenKind::LongDoubleLiteral:
		return parseNodeFloating();
	case TokenKind::TrueLiteral:
	case TokenKind::FalseLiteral:
		return parseNodeBoolean();
	case TokenKind::StringLiteral:
	case TokenKind::WStringLiteral:
		return parseNodeString();
	case TokenKind::CharLiteral:
	case TokenKind::WCharLiteral:
		return parseNodeBoolean();
	}
	return nullptr;
}

Node* Parser::parseNodeInteger() {
	return new NodeInteger(stream.consume(stream.peek().type).value);
}

Node* Parser::parseNodeFloating() {
	return new NodeFloating(stream.consume(stream.peek().type).value);
}

Node* Parser::parseNodeBoolean() {
	return new NodeBoolean(stream.consume(stream.peek().type).value);
}

Node* Parser::parseNodeString() {
	return new NodeString(stream.consume(stream.peek().type).value);
}

Node* Parser::parseNodeCharacter() {
	return new NodeCharacter(stream.consume(stream.peek().type).value);
}

Node* Parser::parseIdentifier() {

	std::string Identifier = parse_namespace();

	switch (stream.peek().type)
	{
	case TokenKind::LeftParen:
		return parseNodeCall(Identifier);
	case TokenKind::Equals:
		stream.consume(stream.peek().type);
		return new NodeDeclaration(new NodeIdentifier(Identifier), parseExpression());
	case TokenKind::Dot:
	case TokenKind::Arrow:
		auto Token = stream.consume(stream.peek().type).type;
		return new NodeMemberCall(new NodeIdentifier(Identifier), parseIdentifier(), Token == TokenKind::Arrow);
	}

	return Identifier.empty() ? nullptr : new NodeIdentifier(Identifier);
}

Node* Parser::parseNew() {

	stream.consume(TokenKind::New);
	return new NodeNew(parseIdentifier());
}

Node* Parser::parseDelete() {
	stream.consume(TokenKind::Delete_);
	return new NodeDelete();
}

Node* Parser::parseNullptr() {
	stream.consume(TokenKind::NullptrLiteral);
	return new NodeNullptr();
}

Node* Parser::parseNodeCall(std::string Func) {

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	std::vector<Node*> ArgumentConcreticList;
	ArgumentConcreticList.push_back(parseExpression());
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		ArgumentConcreticList.push_back(parseExpression());
	}

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	return new NodeCall(Func, ArgumentConcreticList);

}

Node* Parser::parseArgument() {

	stream.consume(TokenKind::Var);

	auto Type = parseType();
	if (!Type) return nullptr;

	std::vector<Node*> ContainerDeclarationList;
	ContainerDeclarationList.push_back(parseDeclaration());
	return new NodeDeclarationList(Type, ContainerDeclarationList);
}

std::vector<Node*> Parser::parseArgumentList() {
	std::vector<Node*> ArgumentList;
	ArgumentList.push_back(parseArgument());
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		ArgumentList.push_back(parseArgument());
	}
	return ArgumentList;
}

Node* Parser::parseFunction() {

	stream.consume(TokenKind::Function);

	auto Type = parseType();

	if (!Type)
		return nullptr;

	if (!stream.match(TokenKind::LeftBracket)) {
		// Необязательный — если нет, пропускаем
	}
	else {
		while (!stream.match(TokenKind::RightBracket)) {
			stream.consume(stream.peek().type);
		}
	}

	// Имя функции
	if (stream.peek().type != TokenKind::IdentifierLiteral) {
		return nullptr;
	}
	std::string FunctionName = parse_namespace();
	
	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	std::vector<Node*> ArgumentList = parseArgumentList();

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	// Тело функции или ';'
	Node* body = nullptr;

	switch (stream.peek().type) {
	case TokenKind::LeftBrace:
		stream.consume(TokenKind::LeftBrace);
		body = parseFunctionBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected RightBrace token");
		stream.consume(TokenKind::RightBrace);
		break;
	case TokenKind::Semicolon:
		stream.consume(TokenKind::Semicolon);
		// Прототип функции
		break;
	default:
		throw std::runtime_error("not expected Semicolon or LeftBrace");
	}
	return new NodeFunction(Type, FunctionName, ArgumentList, body);
}

Node* Parser::parseConstructor() {

	stream.consume(TokenKind::Constructor);

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	std::vector<Node*> ArgumentList = parseArgumentList();

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	// Тело функции или ';'
	Node* body = nullptr;

	switch (stream.peek().type) {
	case TokenKind::LeftBrace:
		stream.consume(TokenKind::LeftBrace);
		body = parseFunctionBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected RightBrace token");
		stream.consume(TokenKind::RightBrace);
		break;
	case TokenKind::Semicolon:
		stream.consume(TokenKind::Semicolon);
		// Прототип функции
		break;
	default:
		throw std::runtime_error("not expected Semicolon or LeftBrace");
	}

	return new NodeConstructor(ArgumentList, body);
}

Node* Parser::parseDestructor() {

	stream.consume(TokenKind::Destructor);

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	// Тело функции или ';'
	Node* body = nullptr;

	switch (stream.peek().type) {
	case TokenKind::LeftBrace:
		stream.consume(TokenKind::LeftBrace);
		body = parseFunctionBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected RightBrace token");
		stream.consume(TokenKind::RightBrace);
		break;
	case TokenKind::Semicolon:
		stream.consume(TokenKind::Semicolon);
		// Прототип функции
		break;
	default:
		throw std::runtime_error("not expected Semicolon or LeftBrace");
	}

	return new NodeDestructor(body);
}

Node* Parser::parseFunctionBlock() {

	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:      stmt = parseVar(); break;
		case TokenKind::IdentifierLiteral: stmt = parseIdentifier(); break;
		case TokenKind::Lambda: stmt = parseLambda(); break;
		case TokenKind::While: stmt = parseWhile(); break;
		case TokenKind::Try: stmt = parseTryCatch(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}

	return block;
}

Node* Parser::parseLambda() {

	stream.consume(TokenKind::Lambda);

	auto Type = parseType();

	if (!Type)
		return nullptr;

	// Имя лямбды
	if (stream.peek().type != TokenKind::IdentifierLiteral) {
		return nullptr;
	}
	std::string LambdaName = parse_namespace();

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	std::vector<Node*> ArgumentList = parseArgumentList();

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	// Тело лямбды или ';'
	Node* body = nullptr;

	switch (stream.peek().type) {
	case TokenKind::LeftBrace:
		stream.consume(TokenKind::LeftBrace);
		body = parseFunctionBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected RightBrace token");
		stream.consume(TokenKind::RightBrace);
		break;
	case TokenKind::Semicolon:
		stream.consume(TokenKind::Semicolon);
		// Прототип функции
		break;
	default:
		throw std::runtime_error("not expected Semicolon or LeftBrace");
	}
	return new NodeLambda(Type, LambdaName, ArgumentList, body);
}

Node* Parser::parseWhile() {

	stream.consume(TokenKind::While);

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	Node* Condition = nullptr;
	Node* Body = nullptr;
	bool IsDoWhile = false;

	switch (stream.peek().type) 
	{
	case TokenKind::Var:
		Condition = parseVar(); break;
	case TokenKind::IdentifierLiteral:
		Condition = parseIdentifier(); break;
	default:
		throw std::runtime_error("not correct token");
	}

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	if (stream.match(TokenKind::Do))
		IsDoWhile = true;

	if (stream.peek().type != TokenKind::LeftBrace)
		throw std::runtime_error("Expected '{' after while declaration");
	stream.consume(TokenKind::LeftBrace);

	Body = parseWhileBlock();

	if (stream.peek().type != TokenKind::RightBrace)
		throw std::runtime_error("Expected '}' after while declaration");
	stream.consume(TokenKind::RightBrace);

	return new NodeWhile(Condition, Body, IsDoWhile);
}

Node* Parser::parseWhileBlock() {

	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:      stmt = parseVar(); break;
		case TokenKind::IdentifierLiteral: stmt = parseIdentifier(); break;
		case TokenKind::Lambda: stmt = parseLambda(); break;
		case TokenKind::While: stmt = parseWhile(); break;
		case TokenKind::Try: stmt = parseWhile(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}
	return block;
}

Node* Parser::parseTryCatch() {

	stream.consume(TokenKind::Try);

	Node* TryBody = nullptr;
	Node* CatchBody = nullptr;
	Node* Declaration = nullptr;

	if (stream.peek().type != TokenKind::LeftBrace)
		throw std::runtime_error("Expected LeftBrace token");
	stream.consume(TokenKind::LeftBrace);
	
	TryBody = parseTryBlock();
	
	if (stream.peek().type != TokenKind::RightBrace)
		throw std::runtime_error("Expected RightBrace token");
	stream.consume(TokenKind::RightBrace);

	if (stream.peek().type == TokenKind::Catch)
	{
		stream.consume(TokenKind::Catch);

		if (stream.peek().type == TokenKind::LeftParen)
		{
			stream.consume(TokenKind::LeftParen);

			if (stream.peek().type != TokenKind::Var)
				throw std::runtime_error("Expected Var token");

			Declaration = parseVar();

			if (stream.peek().type != TokenKind::RightParen)
				throw std::runtime_error("Expected RightParen token");
			stream.consume(TokenKind::RightParen);

		}
		CatchBody = parseWhileBlock();
	}

	return new NodeTryCatch(TryBody, CatchBody, Declaration);
}

Node* Parser::parseTryBlock() {

	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:      stmt = parseVar(); break;
		case TokenKind::IdentifierLiteral: stmt = parseIdentifier(); break;
		case TokenKind::Lambda: stmt = parseLambda(); break;
		case TokenKind::While: stmt = parseWhile(); break;
		case TokenKind::Try: stmt = parseWhile(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}
	return block;
}

Node* Parser::parseCatchBlock() {

	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:      stmt = parseVar(); break;
		case TokenKind::IdentifierLiteral: stmt = parseIdentifier(); break;
		case TokenKind::Lambda: stmt = parseLambda(); break;
		case TokenKind::While: stmt = parseWhile(); break;
		case TokenKind::Try: stmt = parseWhile(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}
	return block;
}

Node* Parser::parseAccess() {
	
	stream.consume(TokenKind::Access);
	
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");

	std::string Name = stream.consume(TokenKind::IdentifierLiteral).value;

	if (stream.peek().type != TokenKind::Equals)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Equals);

	// Пофиг, пропускаем
	std::string Path = parse_namespace();

	if (stream.peek().type != TokenKind::Semicolon)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Semicolon);

	// Temporary stub
	return new NodeAccess(Name, Path);
};

Node* Parser::parseUsing() {
	
	stream.consume(TokenKind::Using);

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");

	std::string Name = stream.consume(TokenKind::IdentifierLiteral).value;

	if (stream.peek().type != TokenKind::Equals)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Equals);

	// Пофиг, пропускаем
	std::string Path = parse_namespace();

	if (stream.peek().type != TokenKind::Semicolon)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Semicolon);

	// Temporary stub
	return new NodeUsing(Name, Path);
};

Node* Parser::parseGenericParametrs() {

	NodeGenericParams* genericParams = nullptr;

	if (stream.match(TokenKind::LeftBracket)) {
		genericParams = new NodeGenericParams();

		while (true) {
			// Имя параметра
			if (stream.peek().type != TokenKind::IdentifierLiteral)
				throw std::runtime_error("Expected generic parameter name");
			std::string paramName = stream.consume(TokenKind::IdentifierLiteral).value;

			// Default value (опционально)
			Node* defaultExpr = nullptr;
			if (stream.match(TokenKind::Equals)) {
				if (stream.peek().type == TokenKind::IdentifierLiteral)
				{
					// Значение может быть: int, std::string — т.е. namespace/identifier
					defaultExpr = parseExpression();
				}
			}

			bool construct = false;
			if (stream.match(TokenKind::LeftParen))
			{
				// Временно поддерживаем только () чтобы не усложнять парсинг
				// Хотя по хорошему необходимо вызывать парсинг аргументов функции
				stream.match(TokenKind::LeftParen);
				if (!stream.match(TokenKind::RightParen))
					throw std::runtime_error("Expected RightParen token");
				stream.match(TokenKind::LeftParen);
				construct = true;
			}

			genericParams->add(new NodeGenericParam(paramName, construct, defaultExpr));

			if (stream.match(TokenKind::RightBracket))
				break;
			if (!stream.match(TokenKind::Comma))
				throw std::runtime_error("Expected ',' or ']' in generic parameters");
		}
	}

	return genericParams;
}

Node* Parser::parseGenericParametrsConcretic() {

	NodeGenericParamsConcretic* genericParamsConcretic = nullptr;

	if (stream.match(TokenKind::LeftBracket)) {
		genericParamsConcretic = new NodeGenericParamsConcretic();
		while (true) {
			std::string arg;
			// Парсим аргумент: int, 5, std::string (namespace)
			if (stream.peek().type == TokenKind::IdentifierLiteral)
			{
				genericParamsConcretic->add(new NodeIdentifier(parse_namespace()));
			}
			else
			{
				// Не заморачиваемся с выражениями
				genericParamsConcretic->add(new NodeIdentifier(stream.consume(stream.peek().type).value));
			}
			if (stream.match(TokenKind::RightBracket)) {
				break;
			}
			if (!stream.match(TokenKind::Comma))
				throw std::runtime_error("Expected ',' or ']' in generic arguments");
		}
	}
	return genericParamsConcretic;
}

Node* Parser::parseClass() {
	// assume current token is Class
	stream.consume(TokenKind::Class);

	std::string name;
	if (stream.peek().type == TokenKind::IdentifierLiteral)
		name = stream.consume(TokenKind::IdentifierLiteral).value;
	else
		throw std::runtime_error("Expected class name");

	// Generic-параметры: [T, K = int]
	Node* genericParams = parseGenericParametrs();
	// Поддержка generic-конкретизации: [int, std::string]
	Node* genericParamsConcretic = nullptr;
	std::string baseClass;
	NodeClass::INHERITANCE_TYPE inheritanceType = NodeClass::INHERITANCE_TYPE::PRIVATE;

	if (stream.match(TokenKind::Colon)) {
		// Проверяем public/private
		if (stream.peek().type == TokenKind::Public) {
			inheritanceType = NodeClass::INHERITANCE_TYPE::PUBLIC;
			stream.consume(TokenKind::Public);
		}
		else if (stream.peek().type == TokenKind::Private) {
			inheritanceType = NodeClass::INHERITANCE_TYPE::PRIVATE;
			stream.consume(TokenKind::Private);
		}

		// Имя базового класса
		if (stream.peek().type == TokenKind::IdentifierLiteral) {
			baseClass = stream.consume(TokenKind::IdentifierLiteral).value;
		}
		else {
			throw std::runtime_error("Expected base class name");
		}
		genericParamsConcretic = parseGenericParametrsConcretic();
	}

	Node* body = nullptr;
	if (stream.peek().type != TokenKind::LeftBrace)
		throw std::runtime_error("Expected '{' after class declaration");
	stream.consume(TokenKind::LeftBrace);

	body = parseClassBlock();

	if (stream.peek().type != TokenKind::RightBrace)
		throw std::runtime_error("Expected '}' after class declaration");
	stream.consume(TokenKind::RightBrace);

	return new NodeClass(name, genericParams, genericParamsConcretic, baseClass, inheritanceType, body);
}

Node* Parser::parseClassBlock() {
	
	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:      stmt = parseVar(); break;
		case TokenKind::Function: stmt = parseFunction(); break;
		case TokenKind::Class:    stmt = parseClass(); break;
		case TokenKind::Constructor: stmt = parseConstructor(); break;
		case TokenKind::Destructor: stmt = parseDestructor(); break;
		case TokenKind::Property: stmt = parseProperty(); break;
		case TokenKind::Struct:   stmt = parseStruct(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}
	return block;
}

Node* Parser::parseStruct() {
	// assume current token is Struct
	stream.consume(TokenKind::Struct);

	std::string name;
	if (stream.peek().type == TokenKind::IdentifierLiteral)
		name = stream.consume(TokenKind::IdentifierLiteral).value;
	else
		throw std::runtime_error("Expected class name");

	// Generic-параметры: [T, K = int]
	Node* genericParams = parseGenericParametrs();

	NodeStruct::INHERITANCE_TYPE inheritanceType = NodeStruct::INHERITANCE_TYPE::PUBLIC;
	Node* body = nullptr;
	if (stream.peek().type != TokenKind::LeftBrace)
		throw std::runtime_error("Expected '{' after struct declaration");
	stream.consume(TokenKind::LeftBrace);

	body = parseClassBlock();

	if (stream.peek().type != TokenKind::RightBrace)
		throw std::runtime_error("Expected '}' after struct declaration");
	stream.consume(TokenKind::RightBrace);

	return new NodeStruct(name, genericParams, inheritanceType, body);
}

Node* Parser::parseStructBlock() {

	NodeBlock* block = new NodeBlock();

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Var:      stmt = parseVar(); break;
		case TokenKind::Function: stmt = parseFunction(); break;
		case TokenKind::Class:    stmt = parseClass(); break;
		case TokenKind::Struct:   stmt = parseStruct(); break;
		default:
			stream.consume(stream.peek().type);
			break;
		}
		if (stmt) block->add(stmt);
	}
	return block;
}

Parser::~Parser()
{
	for (auto& i : ast)
		if (i) delete i;
}

#endif // PARSER_HPP