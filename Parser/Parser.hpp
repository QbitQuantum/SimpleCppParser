
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
	Node* parseNamespaceBlock();
	Node* parseTryBlock();
	Node* parseCatchBlock();

	Node* parseIdentifier();

	Node* parseDeclaration();
	Node* parseDeclarationPrimary();

	Node* parseTemplateParameterInstantiation();
	Node* parseTemplateParameterInstantiationList();
	Node* parseTemplateParameterDeclaration();
	Node* parseTemplateParameterDeclarationList();

	// Парсинг объяление шаблонного типа
	Node* parseTemplate();
	Node* parseTemplateTemplateParameterDeclarationList();
	Node* parseTemplateName();
	Node* parseTemplateParameterList();

	Node* ParseTemplateUsing();
	Node* ParseTemplatePointer();
	Node* ParseTemplateVar();
	Node* ParseTemplateIdentifier();

	// Парсинг объяление типов
	Node* parseUsing();
	Node* parseUsingTemplateParameterDeclarationList();
	Node* parseUsingName();
	Node* parseUsingScopeType();
	Node* parseUsingIdentifier();

	// Парсинг объяление сигнатуры функций
	Node* parsePointer();
	Node* parsePointerTemplateParameterDeclarationList();
	Node* parsePointerName();
	Node* parsePointerDeclaration();
	Node* parsePointerIdentifier();
	Node* parsePointerSignature();
	Node* parsePointerReturnType();
	Node* parsePointerParameterList();
	
	// Парсинг объяление пространства имён
	Node* parseAccess();
	Node* parseAccessName();
	Node* parseAccessScope();

	// Парсинг объяление переменных
	Node* parseVar();
	Node* parseVarTemplateParameterDeclarationList();
	Node* parseVarType();
	Node* parseVarDeclaration();
	Node* parseVarDeclarationList();

	// Парсинг класса
	Node* parseClass();
	Node* parseClassName();
	Node* parseClassBody();
	Node* parseClassBaseClass();
	Node* parseClassBlock();
	Node* parseClassTemplateParameterDeclarationList();

	// Парсинг структур (POD-типы)
	Node* parseStruct();
	Node* parseStructName();
	Node* parseStructBody();
	Node* parseStructBlock();
	Node* parseStructTemplateParameterDeclarationList();

	// Парсинг функций
	Node* parseFunction();
	Node* parseFunctionTemplateParameterDeclarationList();
	Node* parseFunctionReturnType();
	Node* parseFunctionQulifier();
	Node* parseFunctionName();
	Node* parseFunctionParameter();
	Node* parseFunctionParameterList();
	Node* parseFunctionBody();
	Node* parseFunctionBlock();

	// Парсинг лямбды
	Node* parseLambda();
	Node* parseLambdaTemplateParameterDeclarationList();
	Node* parseLambdaReturnType();
	Node* parseLambdaName();
	Node* parseLambdaParameterList();
	Node* parseLambdaBody();

	// Парсинг конструктора
	Node* parseConstructor();
	Node* parseConstructorTemplateParameterDeclarationList();
	Node* parseConstructorQulifier();
	Node* parseConstructorParameterList();
	Node* parseConstructorBody();

	// Парсинг деструктора
	Node* parseDestructor();
	Node* parseDestructorQulifier();
	Node* parseDestructorParameterList();
	Node* parseDestructorBody();

	// Парсинг проперти-методов
	Node* parseProperty();
	Node* parsePropertyReturnType();
	Node* parsePropertyName();
	Node* parsePropertyBody();
	Node* parsePropertyBlock();

	// Парсинг while/while do-циклов
	Node* parseWhile();
	Node* parseWhileCondition();
	Node* parseWhileBody();
	Node* parseWhileBlock();

	Node* parseNodeCall(Node* Func);
	Node* parseNamespace();
	Node* parseTryCatch();

	Node* parsePrimary();
	Node* parseExpression(int priory = 0);

	Node* parseNew();
	Node* parseDelete();
	Node* parseNullptr();
	Node* parseDefault();
	Node* parseNodeInteger();
	Node* parseNodeFloating();
	Node* parseNodeBoolean();
	Node* parseNodeString();
	Node* parseNodeCharacter();

	Node* parseType();

	Node* parseIdeitfierScope();
	Node* parseScope();

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
	case TokenKind::Pointer:  return parsePointer();
	case TokenKind::Var:      return parseVar();
	case TokenKind::Function: return parseFunction();
	case TokenKind::Class:    return parseClass();
	case TokenKind::Struct:    return parseStruct();
	case TokenKind::IdentifierLiteral: return parseIdentifier();
	case TokenKind::Namespace: return parseNamespace();
	case TokenKind::Template: return parseTemplate();
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
	Node* Name = parseIdeitfierScope();

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

Node* Parser::parseIdeitfierScope() {
	std::string Identifier = "";
	std::vector<std::string> Scope;
	Node* IdentifierTemplateParameterInstantiationList = nullptr;
	while (true) {
		switch (stream.peek().type) {
		case TokenKind::IdentifierLiteral:
			Identifier = stream.consume(TokenKind::IdentifierLiteral).value;
			break;
		case TokenKind::ScResOp:
			stream.consume(TokenKind::ScResOp);
			if (stream.peek().type != TokenKind::IdentifierLiteral)
				throw std::runtime_error("Expected identifier after '::'");
			Scope.push_back(Identifier);
			Identifier = "";
			break;
		case TokenKind::Less:
			IdentifierTemplateParameterInstantiationList = parseTemplateParameterInstantiationList();
			return new NodeIdentifier(IdentifierTemplateParameterInstantiationList, Identifier, new NodeScope(Scope));
			break;
		default:
			return new NodeIdentifier(IdentifierTemplateParameterInstantiationList, Identifier, new NodeScope(Scope));
		}
	}
}

Node* Parser::parseScope() {
	std::vector<std::string> scope;
	while (true) {
		switch (stream.peek().type) {
		case TokenKind::IdentifierLiteral:
			scope.push_back(stream.consume(TokenKind::IdentifierLiteral).value);
			break;
		case TokenKind::ScResOp:
			stream.consume(TokenKind::ScResOp);
			if (stream.peek().type != TokenKind::IdentifierLiteral)
				throw std::runtime_error("Expected identifier after '::'");
			break;
		default:
			return new NodeScope(scope);
		}
	}
}

Node* Parser::parseType() {

	/*
	Допустимые вариации типов
	T&          // изменяемая ссылка
	const T&    // неизменяемая ссылка
	T*          // указатель
	const T*    // указатель на константу
	T&&         // rvalue-ссылка (move)
	const T&&   // - бессмысленно, но для простоты парсинга
	*/


	stream.consume(TokenKind::LeftBracket);

	Node* Type = nullptr;
	bool IsConst = false;
	NodeType::EType eType = NodeType::EType::NONE;

	// Проверям на константность
	if (stream.match(TokenKind::Const))
		IsConst = true;

	// Проверяем наличие типа
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected identifier token");

	// Парсим имя типа
	Type = parseIdeitfierScope();

	// Проверяем семантику 
	switch (stream.peek().type)
	{
	case TokenKind::Asterisk:
		stream.consume(TokenKind::Asterisk);
		eType = NodeType::EType::POINTER;
		break;
	case TokenKind::Ampersand:
		stream.consume(TokenKind::Ampersand);
		eType = NodeType::EType::REF;
		break;
	case TokenKind::And:
		stream.consume(TokenKind::And);
		eType = NodeType::EType::RVALUE;
		break;
	default:
		break;
	}

	if (!stream.match(TokenKind::RightBracket))
		throw std::runtime_error("Expected RightBracket token");

	return new NodeType(Type, IsConst, eType);
};

Node* Parser::parseDeclaration() {

	Node* Identifier = nullptr;
	Node* Exptression = nullptr;

	// Имя может быть пустое. По хорошему исключить такую фигню
	if (stream.peek().type == TokenKind::IdentifierLiteral)
		// То что может быть Namespace::Name в имене идентикатора - работа семантера
		Identifier = parseIdeitfierScope();

	if (stream.peek().type == TokenKind::Equals)
	{
		if (!Identifier)
			throw std::runtime_error("Expected identifier");
		stream.consume(TokenKind::Equals);
		Exptression = parseExpression();
	}

	return new NodeDeclaration(Identifier, Exptression);
}

Node* Parser::parseDeclarationPrimary() {

	Node* Identifier = nullptr;
	Node* Exptression = nullptr;

	// Имя может быть пустое. По хорошему исключить такую фигню
	if (stream.peek().type == TokenKind::IdentifierLiteral)
		// То что может быть Namespace::Name в имене идентикатора - работа семантера
		Identifier = parseIdeitfierScope();

	if (stream.peek().type == TokenKind::Equals)
	{
		if (!Identifier)
			throw std::runtime_error("Expected identifier");
		stream.consume(TokenKind::Equals);
		Exptression = parsePrimary();
	}

	return new NodeDeclaration(Identifier, Exptression);
}

Node* Parser::parseUsing() {

	stream.consume(TokenKind::Using);

	Node* UsingTemplateParameterDeclarationList = parseUsingTemplateParameterDeclarationList();

	Node* UsingName = parseUsingName();

	Node* UsingScopeType = parseUsingScopeType();

	return new NodeUsing(UsingTemplateParameterDeclarationList, UsingName, UsingScopeType);
};

Node* Parser::parseUsingTemplateParameterDeclarationList() {
	Node* UsingTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		UsingTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return UsingTemplateParameterDeclarationList;
};

Node* Parser::parseUsingName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected Equals token");
	return parseIdeitfierScope();
};

Node* Parser::parseUsingScopeType() {
	Node* ParseScopeType = nullptr;
	if (stream.match(TokenKind::Equals))
		ParseScopeType = parseUsingIdentifier();
	return ParseScopeType;
};

Node* Parser::parseUsingIdentifier() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
};

Node* Parser::parsePointer() {

	stream.consume(TokenKind::Pointer);

	Node* PointerTemplateParameterDeclarationList = parsePointerTemplateParameterDeclarationList();
	
	Node* PointerName = parsePointerName();

	Node* PointerDeclaration = parsePointerDeclaration();

	return new NodePointer(PointerTemplateParameterDeclarationList, PointerName, PointerDeclaration);
};

Node* Parser::parsePointerTemplateParameterDeclarationList() {
	Node* PointerTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		PointerTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return PointerTemplateParameterDeclarationList;
};

Node* Parser::parsePointerName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
};

Node* Parser::parsePointerDeclaration() {
	if (stream.peek().type != TokenKind::Equals)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Equals);

	return stream.peek().type == TokenKind::IdentifierLiteral ?
		parsePointerIdentifier() : parsePointerSignature();
};

Node* Parser::parsePointerIdentifier() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
};

Node* Parser::parsePointerSignature() {

	Node* PointerReturnType = parsePointerReturnType();

	Node* PointerParameterList = parsePointerParameterList();

	return new NodePointerSignature(PointerReturnType, PointerParameterList);
};

Node* Parser::parsePointerReturnType() {
	return parseFunctionReturnType();
};

Node* Parser::parsePointerParameterList() {
	return parseFunctionParameterList();
};

Node* Parser::parseAccess() {

	stream.consume(TokenKind::Access);

	Node* AccessName = parseAccessName();

	Node* AccessScope = parseAccessScope();

	// Temporary stub
	return new NodeAccess(AccessName, AccessScope);
};

Node* Parser::parseAccessName() {

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
};

Node* Parser::parseAccessScope() {

	if (stream.peek().type != TokenKind::Equals)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Equals);

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected Equals token");

	return parseScope();
};

Node* Parser::parseVar() {

	stream.consume(TokenKind::Var);

	Node* VarTemplateParameterDeclarationList = parseVarTemplateParameterDeclarationList();

	Node* VarType = parseVarType();

	Node* VarDeclarationList = parseVarDeclarationList();

	return new NodeVarDeclarationList(VarTemplateParameterDeclarationList, VarType, VarDeclarationList);
}

Node* Parser::parseVarTemplateParameterDeclarationList() {
	Node* VarTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		VarTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return VarTemplateParameterDeclarationList;
}

Node* Parser::parseVarType() {
	if (!stream.match(TokenKind::LeftBracket))
		throw std::runtime_error("Expected LeftBracket token");
	return parseType();
}

Node* Parser::parseVarDeclaration() {
	return parseDeclaration();
}

Node* Parser::parseVarDeclarationList() {
	std::vector<Node*> ContainerDeclarationList;
	ContainerDeclarationList.push_back(parseDeclaration());
	// Парсим аргументы: name = default
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		ContainerDeclarationList.push_back(parseDeclaration());
	}
	return new NodeDeclarationList(ContainerDeclarationList);
}

Node* Parser::parsePrimary() {

	using UnaryOperand = NodeUnaryOp::UnaryOp;
	UnaryOperand UnaryOp = UnaryOperand::Unknown;

	auto getUnaryOperand = [](TokenKind op) -> UnaryOperand
		{
			switch (op) {
			case TokenKind::Minus: return UnaryOperand::Minus;
			default: return UnaryOperand::Unknown;
			}
		};

	if (tok::IsUnaryOperator(stream.peek().type))
	{
		UnaryOp = getUnaryOperand(stream.peek().type);
		stream.consume(stream.peek().type);
	}

	Node* Right = nullptr;

	switch (stream.peek().type) {
	case TokenKind::New:
		Right = parseNew(); break;
	case TokenKind::Delete_:
		Right = parseDelete(); break;
	case TokenKind::NullptrLiteral:
		Right = parseNullptr(); break;
	case TokenKind::Default:
		Right = parseDefault(); break;
	case TokenKind::IdentifierLiteral:
		Right = parseIdentifier(); break;
	case TokenKind::IntegerLiteral:
	case TokenKind::HexLiteral:
	case TokenKind::BinaryLiteral:
		Right = parseNodeInteger(); break;
	case TokenKind::FloatLiteral:
	case TokenKind::DoubleLiteral:
	case TokenKind::LongDoubleLiteral:
		Right = parseNodeFloating(); break;
	case TokenKind::TrueLiteral:
	case TokenKind::FalseLiteral:
		Right = parseNodeBoolean(); break;
	case TokenKind::StringLiteral:
	case TokenKind::WStringLiteral:
		Right = parseNodeString(); break;
	case TokenKind::CharLiteral:
	case TokenKind::WCharLiteral:
		Right = parseNodeCharacter(); break;
	case TokenKind::LeftParen:
	{
		stream.consume(TokenKind::LeftParen);
		Right = parseExpression();
		if (stream.peek().type != TokenKind::RightParen) {
			throw std::runtime_error("Expected ')'");
		}
		stream.consume(TokenKind::RightParen);
		break;
	}
	default: throw std::runtime_error("Unexpected token in primary expression");
	}
	return UnaryOp == UnaryOperand::Unknown ? Right : new NodeUnaryOp(UnaryOp, Right);
}

Node* Parser::parseExpression(int MinPrec) {

	using BinaryOperand = NodeBinaryOp::BinaryOp;
	BinaryOperand UnaryOp = BinaryOperand::Unknown;

	auto getBinaryOperand = [](TokenKind op) -> BinaryOperand
		{
			switch (op) {
			case TokenKind::Minus: return BinaryOperand::Minus;
			case TokenKind::Plus: return BinaryOperand::Plus;
			case TokenKind::Asterisk: return BinaryOperand::Asterisk;
			case TokenKind::Slash: return BinaryOperand::Slash;
			default: return BinaryOperand::Unknown;
			}
		};
	
	Node* Left = parsePrimary();

	while (true) {
		TokenKind op = stream.peek().type;
		if (!tok::IsBinaryOperator(op))
			break;
		int currentPriority = tok::GetBinaryOperatorPriority(op);
		if (currentPriority < MinPrec)
			break;
		stream.consume(op);
		Node* Right = parseExpression(currentPriority + 1);
		Left = new NodeBinaryOp(getBinaryOperand(op), Left, Right);
	}

	return Left;
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

Node* Parser::parseDefault() {
	stream.consume(TokenKind::Default);
	return new NodeDefault();
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

	Node* Identifier = parseIdeitfierScope();

	switch (stream.peek().type)
	{
	case TokenKind::LeftParen:
		return parseNodeCall(Identifier);
	case TokenKind::Equals:
		stream.consume(stream.peek().type);
		return new NodeDeclaration(Identifier, parseExpression());
	case TokenKind::Dot:
	case TokenKind::Arrow:
		auto Token = stream.consume(stream.peek().type).type;
		return new NodeMemberCall(Identifier, parseIdentifier(), Token == TokenKind::Arrow);
	}

	return Identifier;
}

Node* Parser::parseNodeCall(Node* Func) {

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	std::vector<Node*> ArgumentConcreticList;

	if (stream.peek().type != TokenKind::RightParen)
	{
		ArgumentConcreticList.push_back(parseExpression());
		while (stream.peek().type == TokenKind::Comma) {
			stream.consume(TokenKind::Comma);
			ArgumentConcreticList.push_back(parseExpression());
		}
	}

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	return new NodeCall(Func, ArgumentConcreticList);

}

Node* Parser::parseTemplateParameterInstantiation() {
	// TODO.1
	switch (stream.peek().type)
	{
		// Некрасиво так парсить
	case TokenKind::Less:			return parseTemplateTemplateParameterDeclarationList();
	case TokenKind::LeftBracket:	return parsePointerSignature();
	default:						return parsePrimary();
	}
}

Node* Parser::parseTemplateParameterInstantiationList() {

	// Instantiation-параметры: PublicMethodBase<int, std::vector<int>>
	stream.consume(TokenKind::Less);

	std::vector<Node*> TemplateParameterInstantiationList;
	if (stream.peek().type != TokenKind::Greater)
	{
		TemplateParameterInstantiationList.push_back(parseTemplateParameterInstantiation());
		while (stream.peek().type == TokenKind::Comma) {
			stream.consume(TokenKind::Comma);
			TemplateParameterInstantiationList.push_back(parseTemplateParameterInstantiation());
		}
	}
	if (stream.peek().type != TokenKind::Greater)
		throw std::runtime_error("Expected Greater token");
	stream.consume(TokenKind::Greater);

	return new NodeTemplateParameterInstantiationList(TemplateParameterInstantiationList);
}

Node* Parser::parseTemplateParameterDeclaration() {
	switch (stream.peek().type) {
	case TokenKind::Using:				return ParseTemplateUsing();
	case TokenKind::Template:			return parseTemplate();
	case TokenKind::Pointer:			return ParseTemplatePointer();
	case TokenKind::Var:				return ParseTemplateVar();
	case TokenKind::IdentifierLiteral:	return ParseTemplateIdentifier();
	default: throw std::runtime_error("Expected stream.peek().type declaration");
	}
}

Node* Parser::parseTemplateParameterDeclarationList() {
	stream.consume(TokenKind::Less);

	std::vector<Node*> TemplateParameterList;
	if (stream.peek().type != TokenKind::Greater)
	{
		TemplateParameterList.push_back(parseTemplateParameterDeclaration());
		while (stream.peek().type == TokenKind::Comma) {
			stream.consume(TokenKind::Comma);
			TemplateParameterList.push_back(parseTemplateParameterDeclaration());
		}
	}
	
	if (stream.peek().type != TokenKind::Greater)
		throw std::runtime_error("Expected Greater token");
	stream.consume(TokenKind::Greater);

	return new NodeTemplateParameterDeclartionList(TemplateParameterList);
}

Node* Parser::parseTemplate() {

	stream.consume(TokenKind::Template);

	Node* TemplateTemplateParameterDeclarationList = parseUsingTemplateParameterDeclarationList();

	Node* TemplateName = parseTemplateName();

	Node* TemplatePrameterList = parseTemplateParameterList();

	return new NodeTemplate(TemplateTemplateParameterDeclarationList, TemplateName, TemplatePrameterList);
};

Node* Parser::parseTemplateTemplateParameterDeclarationList() {
	Node* TemplateTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		TemplateTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return TemplateTemplateParameterDeclarationList;
};

Node* Parser::parseTemplateName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
};

Node* Parser::parseTemplateParameterList() {
	Node* DeclarationList = nullptr;
	if (stream.match(TokenKind::Equals))
	{
		std::vector<Node*> TemplateParameterList;
		TemplateParameterList.push_back(parseTemplateParameterDeclaration());
		while (stream.peek().type == TokenKind::Comma) {
			stream.consume(TokenKind::Comma);
			TemplateParameterList.push_back(parseTemplateParameterDeclaration());
		}
		DeclarationList = new NodeDeclarationList(TemplateParameterList);
	}
	return DeclarationList;
};

Node* Parser::ParseTemplateUsing() {
	if (!stream.match(TokenKind::Using))
		throw std::runtime_error("Expected IdentifierLiteral token");

	Node* UsingName = parseUsingName();

	Node* UsingScopeType = parseUsingScopeType();

	return new NodeUsing(nullptr, UsingName, UsingScopeType);
};

Node* Parser::ParseTemplatePointer() {
	if (!stream.match(TokenKind::Pointer))
		throw std::runtime_error("Expected Pointer token");
	stream.consume(TokenKind::Pointer);

	Node* PointerName = parsePointerName();

	Node* PointerDeclaration = parsePointerDeclaration();

	return new NodePointer(nullptr, PointerName, PointerDeclaration);
};

Node* Parser::ParseTemplateVar() {

	if (stream.peek().type != TokenKind::Var)
		throw std::runtime_error("Expected Var token");
	stream.consume(TokenKind::Var);

	Node* VarType = parseVarType();

	Node* VarDeclarationList = parseDeclarationPrimary();

	return new NodeVarDeclarationList(nullptr, VarType, VarDeclarationList);
};

Node* Parser::ParseTemplateIdentifier() {

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected Var token");

	Node* UsingName = parseUsingName();

	Node* UsingScopeType = parseUsingScopeType();

	return new NodeUsingSimple(UsingName, UsingScopeType);
};

Node* Parser::parseClass() {
	// assume current token is Class
	stream.consume(TokenKind::Class);

	Node* ClassTemplateParameterDeclarationList = parseClassTemplateParameterDeclarationList();

	Node* ClassName = parseClassName();

	Node* ClassBaseClass = parseClassBaseClass();

	Node* ClassBody = parseClassBody();

	return new NodeClass(ClassName, ClassTemplateParameterDeclarationList, ClassBaseClass, ClassBody);
}

Node* Parser::parseClassName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected class name");
	return parseIdeitfierScope();
}

Node* Parser::parseClassBody() {
	
	Node* Body = nullptr;

	if (stream.peek().type == TokenKind::LeftBrace)
	{
		stream.consume(TokenKind::LeftBrace);
		Body = parseClassBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected '}' after class declaration");
		stream.consume(TokenKind::RightBrace);
	}
	else
	{
		if (stream.peek().type != TokenKind::Semicolon)
			throw std::runtime_error("Expected ';' after class forward declaration");
	}

	return Body;
}

Node* Parser::parseClassBaseClass() {
	Node* BaseClass = nullptr;

	if (stream.match(TokenKind::Colon))
	{
		using ClassInheritanceType = NodeBaseClass::InheritanceType;
		ClassInheritanceType Type = ClassInheritanceType::NONE;

		switch (stream.peek().type)
		{
		case TokenKind::Public: 
			stream.consume(TokenKind::Public);
			Type = ClassInheritanceType::PUBLIC;
			break;
		case TokenKind::Private: 
			stream.consume(TokenKind::Private);
			Type = ClassInheritanceType::PRIVATE;
			break;
		default:
			Type = ClassInheritanceType::NONE; break;
		}

		Node* ClassName = parseClassName();

		BaseClass = new NodeBaseClass(ClassName, Type);
	}
	return BaseClass;
}

Node* Parser::parseClassBlock() {

	// Ужас. Надо будет переделать
	using ClassFieldType = NodeBlockClass::FieldType;
	std::vector<Node*> Statements;
	std::vector<std::pair<ClassFieldType, std::vector<Node*>>> FieldStatements;
	ClassFieldType Type = ClassFieldType::NONE;

	auto getClassFieldType = [](TokenKind op) -> ClassFieldType
		{
			switch (op) {
			case TokenKind::Private: return ClassFieldType::PRIVATE;
			case TokenKind::Public: return ClassFieldType::PUBLIC;
			case TokenKind::Static: return ClassFieldType::STATIC;
			default: return ClassFieldType::NONE;
			}
		};

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Private:
		case TokenKind::Public:
		case TokenKind::Static:
		{
			TokenKind Scope = stream.peek().type;
			if (!Statements.empty() || Type != ClassFieldType::NONE) {
				FieldStatements.push_back({ Type, Statements });
				Statements.clear();
			}
			Type = getClassFieldType(Scope);
			stream.consume(Scope);
			break;
		}
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
		if (stmt) Statements.push_back(stmt);
	}

	if (!Statements.empty() || Type != ClassFieldType::NONE) {
		FieldStatements.push_back({ Type, Statements });
	}

	return new NodeBlockClass(FieldStatements);
}

Node* Parser::parseClassTemplateParameterDeclarationList() {
	Node* ClassTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		ClassTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return ClassTemplateParameterDeclarationList;
}

Node* Parser::parseStruct() {
	// assume current token is Struct
	stream.consume(TokenKind::Struct);

	Node* StructTemplateParameterDeclarationList = parseStructTemplateParameterDeclarationList();

	Node* StructName = parseStructName();

	Node* StructBody = parseStructBody();

	return new NodeStruct(StructName, StructTemplateParameterDeclarationList, StructBody);
}

Node* Parser::parseStructTemplateParameterDeclarationList() {
	Node* StructTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		StructTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return StructTemplateParameterDeclarationList;
}

Node* Parser::parseStructName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected struct name");
	return parseIdeitfierScope();
}

Node* Parser::parseStructBody() {

	Node* Body = nullptr;

	if (stream.peek().type == TokenKind::LeftBrace)
	{
		stream.consume(TokenKind::LeftBrace);
		Body = parseStructBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected '}' after struct declaration");
		stream.consume(TokenKind::RightBrace);
	}
	else
	{
		if (stream.peek().type != TokenKind::Semicolon)
			throw std::runtime_error("Expected ';' after struct forward declaration");
	}

	return Body;
}

Node* Parser::parseStructBlock() {
	using StructFieldType = NodeBlockStruct::FieldType;
	std::vector<Node*> Statements;
	std::vector<std::pair<StructFieldType, std::vector<Node*>>> FieldStatements;
	StructFieldType Type = StructFieldType::NONE;

	auto getStructFieldType = [](TokenKind op) -> StructFieldType {
		switch (op) {
		case TokenKind::Public: return StructFieldType::PUBLIC;
		case TokenKind::Static: return StructFieldType::STATIC;
		default: return StructFieldType::NONE;
		}
		};

	while (!stream.eof() && stream.peek().type != TokenKind::RightBrace) {
		Node* stmt = nullptr;
		switch (stream.peek().type) {
		case TokenKind::Public:
		case TokenKind::Static:
		{
			TokenKind Scope = stream.peek().type;
			if (!Statements.empty() || Type != StructFieldType::NONE) {
				FieldStatements.push_back({ Type, Statements });
				Statements.clear();
			}
			Type = getStructFieldType(Scope);
			stream.consume(Scope);
			break;
		}
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
		if (stmt) Statements.push_back(stmt);
	}

	if (!Statements.empty() || Type != StructFieldType::NONE) {
		FieldStatements.push_back({ Type, Statements });
	}

	return new NodeBlockStruct(FieldStatements);
}

Node* Parser::parseFunction() {

	stream.consume(TokenKind::Function);

	Node* FunctionTemplateParameterDeclarationList = parseFunctionTemplateParameterDeclarationList();

	Node* FunctionType = parseFunctionReturnType();

	Node* FunctionQuliafier = parseFunctionQulifier();

	Node* FunctionName = parseFunctionName();

	Node* FunctionParameterList = parseFunctionParameterList();

	Node* FunctionBody = parseFunctionBody();

	return new NodeFunction(FunctionType, FunctionTemplateParameterDeclarationList, FunctionName, FunctionParameterList, FunctionBody);
}

Node* Parser::parseFunctionName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
}

Node* Parser::parseFunctionTemplateParameterDeclarationList() {
	Node* FunctionTemplateParameterDeclarationList = nullptr;
	if (stream.peek().type == TokenKind::Less)
		FunctionTemplateParameterDeclarationList = parseTemplateParameterDeclarationList();
	return FunctionTemplateParameterDeclarationList;
}

Node* Parser::parseFunctionReturnType() {
	if (!stream.match(TokenKind::LeftBracket))
		throw std::runtime_error("Expected LeftBracket token");
	return parseType();
}

Node* Parser::parseFunctionQulifier() {
	if (!stream.match(TokenKind::LeftBracket)) {
		// Необязательный — если нет, пропускаем
	}
	else {
		while (!stream.match(TokenKind::RightBracket)) {
			stream.consume(stream.peek().type);
		}
	}
	return nullptr;
}

Node* Parser::parseFunctionBody() {

	Node* Body = nullptr;

	if (stream.match(TokenKind::LeftBrace))
	{
		Body = parseFunctionBlock();
		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected RightBrace token");
		stream.consume(TokenKind::RightBrace);
	}
	else
	{
		if (stream.peek().type != TokenKind::Semicolon)
			throw std::runtime_error("not expected Semicolon token");
		stream.consume(TokenKind::Semicolon);
	}
	return Body;
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

Node* Parser::parseFunctionParameter() {

	if (stream.peek().type != TokenKind::Var)
		throw std::runtime_error("Expected Var token");
	stream.consume(TokenKind::Var);

	Node* FunctionVar = parseType();

	Node* FunctionDeclartion = parseDeclaration();

	return new NodeVarDeclarationList(nullptr, FunctionVar, FunctionDeclartion);
}

Node* Parser::parseFunctionParameterList() {

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	std::vector<Node*> ArgumentList;

	if (stream.peek().type != TokenKind::RightParen)
	{
		ArgumentList.push_back(parseFunctionParameter());
		while (stream.peek().type == TokenKind::Comma) {
			stream.consume(TokenKind::Comma);
			ArgumentList.push_back(parseFunctionParameter());
		}
	}

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	return new NodeParameterList(ArgumentList);
}

Node* Parser::parseLambda() {

	stream.consume(TokenKind::Lambda);

	Node* LambdaTemplateParameterDeclarationList = parseLambdaTemplateParameterDeclarationList();

	Node* LambdaType = parseLambdaReturnType();

	Node* LambdaName = parseLambdaName();

	Node* LambdaParameterList = parseLambdaParameterList();

	Node* LambdaBody = parseLambdaBody();

	return new NodeLambda(LambdaType, LambdaTemplateParameterDeclarationList, LambdaName, LambdaParameterList, LambdaBody);
}

Node* Parser::parseLambdaName() {
	return parseFunctionName();
}

Node* Parser::parseLambdaTemplateParameterDeclarationList() {
	return parseFunctionTemplateParameterDeclarationList();
}

Node* Parser::parseLambdaReturnType() {
	return parseFunctionReturnType();
}

Node* Parser::parseLambdaBody() {
	return parseFunctionBody();
}

Node* Parser::parseLambdaParameterList() {
	return parseFunctionParameterList();
}

Node* Parser::parseConstructor() {

	stream.consume(TokenKind::Constructor);

	Node* ConstructorTemplateParameterDeclarationList = parseConstructorTemplateParameterDeclarationList();

	Node* ConstructorQulifier = parseConstructorQulifier();

	Node* ConstructorParameterList = parseConstructorParameterList();

	Node* ConstructorBody = parseConstructorBody();

	return new NodeConstructor(ConstructorTemplateParameterDeclarationList, ConstructorParameterList, ConstructorBody);
}

Node* Parser::parseConstructorTemplateParameterDeclarationList() {
	return parseFunctionTemplateParameterDeclarationList();
}

Node* Parser::parseConstructorQulifier() {
	if (!stream.match(TokenKind::LeftBracket)) {
		// Необязательный — если нет, пропускаем
	}
	else {
		while (!stream.match(TokenKind::RightBracket)) {
			stream.consume(stream.peek().type);
		}
	}
	return nullptr;
}

Node* Parser::parseConstructorParameterList() {
	return parseFunctionParameterList();
}

Node* Parser::parseConstructorBody() {
	return parseFunctionBody();
}

Node* Parser::parseDestructor() {

	stream.consume(TokenKind::Destructor);

	Node* DestructorQulifier = parseDestructorQulifier();

	Node* DestructorParameterList = parseDestructorParameterList();

	Node* DestructorBody = parseDestructorBody();

	return new NodeDestructor(DestructorParameterList, DestructorBody);
}

Node* Parser::parseDestructorQulifier() {
	if (!stream.match(TokenKind::LeftBracket)) {
		// Необязательный — если нет, пропускаем
	}
	else {
		while (!stream.match(TokenKind::RightBracket)) {
			stream.consume(stream.peek().type);
		}
	}
	return nullptr;
}

Node* Parser::parseDestructorParameterList() {
	
	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	return new NodeParameterList({});
}

Node* Parser::parseDestructorBody() {
	return parseFunctionBody();
}

Node* Parser::parseProperty() {
	
	stream.consume(TokenKind::Property);

	Node* PropertyReturnType = parsePropertyReturnType();

	Node* PropertyName = parsePropertyName();

	Node* PropertyBody = parsePropertyBody();

	return new NodeProperty(PropertyReturnType, PropertyName, PropertyBody);
}

Node* Parser::parsePropertyReturnType() {
	if (!stream.match(TokenKind::LeftBracket))
		throw std::runtime_error("Expected LeftBracket token");
	return parseType();
}

Node* Parser::parsePropertyName() {
	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");
	return parseIdeitfierScope();
}

Node* Parser::parsePropertyBody() {
	if (!stream.match(TokenKind::LeftBrace))
		throw std::runtime_error("Expected '{' in __property");
	stream.consume(TokenKind::LeftBrace);

	Node* Block = parsePropertyBlock();

	if (!stream.match(TokenKind::RightBrace))
		throw std::runtime_error("Expected '}' in __property");
	stream.consume(TokenKind::RightBrace);

	return Block;
}

Node* Parser::parsePropertyBlock() {
	Node* getter = nullptr, * setter = nullptr;

	switch (stream.peek().type)
	{
	case TokenKind::Read:
	{
		stream.consume(TokenKind::Read);
		if (!stream.match(TokenKind::Equals))
			throw std::runtime_error("Expected '{' in __property");
		getter = parseIdeitfierScope();
		break;
	}
	case TokenKind::Write:
	{
		stream.consume(TokenKind::Write);
		if (!stream.match(TokenKind::Equals))
			throw std::runtime_error("Expected '{' in __property");
		setter = parseIdeitfierScope();
		break;
	}
	default:
		break;
	}

	if (stream.match(TokenKind::Comma))
	{
		switch (stream.peek().type)
		{
		case TokenKind::Read:
		{
			if (getter)
				throw std::runtime_error("getter duplicate");
			
			stream.consume(TokenKind::Read);
			if (!stream.match(TokenKind::Equals))
				throw std::runtime_error("Expected '{' in __property");
			getter = parseIdeitfierScope();

			break;
		}
		case TokenKind::Write:
		{
			if (setter)
				throw std::runtime_error("setter duplicate");
			
			stream.consume(TokenKind::Write);
			if (!stream.match(TokenKind::Equals))
				throw std::runtime_error("Expected '{' in __property");
			setter = parseIdeitfierScope();

			break;
		}
		default:
			break;
		}
	}
	return new NodePropertyBlock(getter, setter);
}

Node* Parser::parseWhile() {

	stream.consume(TokenKind::While);

	Node* WhileCondition = parseWhileCondition();

	bool IsDoWhile = false;
	if (stream.match(TokenKind::Do))
		IsDoWhile = true;

	Node* WhileBody = parseWhileBody();

	return new NodeWhile(WhileCondition, WhileBody, IsDoWhile);
}

Node* Parser::parseWhileCondition() {

	if (stream.peek().type != TokenKind::LeftParen)
		throw std::runtime_error("Expected LeftParen token");
	stream.consume(TokenKind::LeftParen);

	Node* WhileCondition = parseExpression();

	if (stream.peek().type != TokenKind::RightParen)
		throw std::runtime_error("Expected RightParen token");
	stream.consume(TokenKind::RightParen);

	return WhileCondition;
}

Node* Parser::parseWhileBody() {

	if (stream.peek().type != TokenKind::LeftBrace)
		throw std::runtime_error("Expected '{' after while declaration");
	stream.consume(TokenKind::LeftBrace);

	Node* WhileBlock = parseWhileBlock();

	if (stream.peek().type != TokenKind::RightBrace)
		throw std::runtime_error("Expected '}' after while declaration");
	stream.consume(TokenKind::RightBrace);

	return WhileBlock;
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
		case TokenKind::Try: stmt = parseTryCatch(); break;
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

	if (stream.match(TokenKind::Catch))
	{
		if (stream.match(TokenKind::LeftParen))
		{
			if (stream.peek().type != TokenKind::Var)
				throw std::runtime_error("Expected Var token");

			Declaration = parseVar();

			if (stream.peek().type != TokenKind::RightParen)
				throw std::runtime_error("Expected RightParen token");
			stream.consume(TokenKind::RightParen);
		}

		if (stream.peek().type != TokenKind::LeftBrace)
			throw std::runtime_error("Expected LeftBrace token");
		stream.consume(TokenKind::LeftBrace);

		CatchBody = parseCatchBlock();

		if (stream.peek().type != TokenKind::RightBrace)
			throw std::runtime_error("Expected RightBrace token");
		stream.consume(TokenKind::RightBrace);
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
		case TokenKind::Try: stmt = parseTryCatch(); break;
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
		case TokenKind::Try: stmt = parseTryCatch(); break;
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