
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
	Node* parseGenericParametrs();
	Node* parseGenericParametrsConcretic();
	Node* parseClass();
	Node* parseBlock();
	Node* parseExpression();
	Node* parseDeclaration();
	Node* parseProperty();

	NodeTypeQualifier* TypeQualifierParse();
	std::string parse_namespace(bool Admissibility = true);
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
	case TTokenID::Property: return parseProperty();
	default:
		return nullptr;
	}
}

Node* Parser::parseProperty() {
	stream.consume(TTokenID::Property); // __property
	
	// тип (int)
	NodeTypeQualifier* typeQualifier = TypeQualifierParse();
	if (!typeQualifier) throw std::runtime_error("Expected type in __property");

	// имя (Value)
	std::string name = stream.consume(TTokenID::IdentifierLiteral).value;

	// {
	if (!stream.match(TTokenID::LeftBrace))
		throw std::runtime_error("Expected '{' in __property");

	std::string getter, setter;
	while (!stream.match(TTokenID::RightBrace)) {
		if (stream.match(TTokenID::Read)) {
			if (!stream.match(TTokenID::Equals))
				throw std::runtime_error("Expected '=' after 'read'");
			getter = parse_namespace(false); // __getValue
		}
		else if (stream.match(TTokenID::Write)) {
			if (!stream.match(TTokenID::Equals))
				throw std::runtime_error("Expected '=' after 'write'");
			setter = parse_namespace(false); // __setValue
		}
		else {
			stream.consume(stream.peek().type);
		}
	}

	return new NodeProperty(name, typeQualifier, getter, setter);
}

std::string Parser::parse_namespace(bool Admissibility) {
	std::string Identifier = "";
	while (true) {
		switch (stream.peek().type) {
		case TTokenID::IdentifierLiteral:
			Identifier = stream.consume(TTokenID::IdentifierLiteral).value;
			break;
		case TTokenID::ScResOp:
			if (!Admissibility)
				throw std::runtime_error("Not admissibility token '::'");
			stream.consume(TTokenID::ScResOp);
			if (stream.peek().type != TTokenID::IdentifierLiteral)
				throw std::runtime_error("Expected identifier after '::'");
			Identifier = "";
			break;
		default:
			return Identifier;
		}
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
		case TTokenID::IdentifierLiteral:
			Type = parse_namespace();
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

	std::vector<Node*> ContainerDeclarationList;

	ContainerDeclarationList.push_back(parseDeclaration());

	// Парсим аргументы: name = default
	while (stream.peek().type == TTokenID::Comma) {
		stream.consume(TTokenID::Comma);
		ContainerDeclarationList.push_back(parseDeclaration());
	}

	stream.consume(TTokenID::Semicolon);

	return new NodeDeclarationList(TypeQualifier, ContainerDeclarationList);
}

Node* Parser::parseDeclaration() {

	NodeIdentifier* Identifier = nullptr;
	Node* Exptression = nullptr;

	// Имя может быть пустое. По хорошему исключить такую фигню
	if (stream.peek().type == TTokenID::IdentifierLiteral)
		// То что может быть Namespace::Name в имене идентикатора - работа семантера
		Identifier = new NodeIdentifier(parse_namespace(false));
	
	if (stream.peek().type == TTokenID::Equals)
	{
		if (!Identifier)
			throw std::runtime_error("Expected identifier");
		stream.consume(TTokenID::Equals);
		Exptression = parseExpression();
	}
	// Пустое выражение "Expression, ,Expression"
	if (!Identifier && !Exptression)
		throw std::runtime_error("Expected identifier and Exptression");

	return new NodeDeclaration(Identifier, Exptression);
}

Node* Parser::parseExpression() {
	std::string Identifier = parse_namespace();
	return new NodeIdentifier(Identifier);
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
				std::vector<Node*> Decls;
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

Node* Parser::parseBlock() {
	if (!stream.match(TTokenID::LeftBrace))
		return nullptr;

	NodeBlock* block = new NodeBlock();

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

Node* Parser::parseGenericParametrs() {

	NodeGenericParams* genericParams = nullptr;

	if (stream.match(TTokenID::LeftBracket)) {
		genericParams = new NodeGenericParams();

		while (true) {
			// Имя параметра
			if (stream.peek().type != TTokenID::IdentifierLiteral)
				throw std::runtime_error("Expected generic parameter name");
			std::string paramName = stream.consume(TTokenID::IdentifierLiteral).value;

			// Default value (опционально)
			Node* defaultExpr = nullptr;
			if (stream.match(TTokenID::Equals)) {
				if (stream.peek().type == TTokenID::IdentifierLiteral)
				{
					// Значение может быть: int, std::string — т.е. namespace/identifier
					defaultExpr = parseExpression();
				}
			}

			bool construct = false;
			if (stream.match(TTokenID::LeftParen))
			{
				// Временно поддерживаем только () чтобы не усложнять парсинг
				// Хотя по хорошему необходимо вызывать парсинг аргументов функции
				stream.match(TTokenID::LeftParen);
				if (!stream.match(TTokenID::RightParen))
					throw std::runtime_error("Expected RightParen token");
				stream.match(TTokenID::LeftParen);
				construct = true;
			}

			genericParams->add(new NodeGenericParam(paramName, construct, defaultExpr));
			
			if (stream.match(TTokenID::RightBracket))
				break;
			if (!stream.match(TTokenID::Comma))
				throw std::runtime_error("Expected ',' or ']' in generic parameters");
		}
	}

	return genericParams;
}

Node* Parser::parseGenericParametrsConcretic() {
	
	NodeGenericParamsConcretic* genericParamsConcretic = nullptr;
	
	if (stream.match(TTokenID::LeftBracket)) {
		genericParamsConcretic = new NodeGenericParamsConcretic();
		while (true) {
			std::string arg;
			// Парсим аргумент: int, 5, std::string (namespace)
			if (stream.peek().type == TTokenID::IdentifierLiteral)
			{
				genericParamsConcretic->add(new NodeIdentifier(parse_namespace()));
			}
			else
			{
				// Не заморачиваемся с выражениями
				genericParamsConcretic->add(new NodeIdentifier(stream.consume(stream.peek().type).value));
			}
			if (stream.match(TTokenID::RightBracket)) {
				break;
			}
			if (!stream.match(TTokenID::Comma))
				throw std::runtime_error("Expected ',' or ']' in generic arguments");
		}
	}
	return genericParamsConcretic;
}

Node* Parser::parseClass() {
	// assume current token is Class
	stream.consume(TTokenID::Class);

	std::string name;
	if (stream.peek().type == TTokenID::IdentifierLiteral)
		name = stream.consume(TTokenID::IdentifierLiteral).value;
	else
		throw std::runtime_error("Expected class name");

	// Generic-параметры: [T, K = int]
	Node* genericParams = parseGenericParametrs();
	// Поддержка generic-конкретизации: [int, std::string]
	Node* genericParamsConcretic = nullptr;
	std::string baseClass;
	NodeClass::INHERITANCE_TYPE inheritanceType = NodeClass::INHERITANCE_TYPE::PRIVATE;

	if (stream.match(TTokenID::Colon)) {
		// Проверяем public/private
		if (stream.peek().type == TTokenID::Public) {
			inheritanceType = NodeClass::INHERITANCE_TYPE::PUBLIC;
			stream.consume(TTokenID::Public);
		}
		else if (stream.peek().type == TTokenID::Private) {
			inheritanceType = NodeClass::INHERITANCE_TYPE::PRIVATE;
			stream.consume(TTokenID::Private);
		}

		// Имя базового класса
		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			baseClass = stream.consume(TTokenID::IdentifierLiteral).value;
		}
		else {
			throw std::runtime_error("Expected base class name");
		}
		genericParamsConcretic = parseGenericParametrsConcretic();
	}

	Node* body = nullptr;
	if (stream.match(TTokenID::LeftBrace)) {
		body = parseBlock();
	}
	else {
		throw std::runtime_error("Expected '{' after class declaration");
	}

	return new NodeClass(name, genericParams, genericParamsConcretic, baseClass, inheritanceType, body);
}

#endif // PARSER_HPP