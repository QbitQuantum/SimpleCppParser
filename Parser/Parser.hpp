
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
	Node* parseGenericParametr();
	Node* parseGenericParametrs();
	Node* parseGenericParametrsConcretic();
	Node* parseClass();
	Node* parseStruct();
	Node* parseConstructor();
	Node* parseDestructor();
	Node* parseExpression(int priory = 0);
	Node* parseDeclaration();
	Node* parseProperty();
	Node* parseNew();
	Node* parseDelete();
	Node* parseNullptr();
	Node* parseNodeCall(Node* Func);
	Node* parseNamespace();
	Node* parseTryCatch();

	Node* parseNodeInteger();
	Node* parseNodeFloating();
	Node* parseNodeBoolean();
	Node* parseNodeString();
	Node* parseNodeCharacter();

	Node* parsePrimary();

	std::vector<Node*> parseArgumentList();
	Node* parseArgument();

	std::vector<Node*> parseTemplateList();
	Node* parseTemplate();


	// Проблема в том, что если это убрать, то получится 
	// var[pair<[int], [vector<[int]>]>] Temporary
	// Что с точки зрения собственного синтаксиса корректно
	// Но нам надо нормальный вид
	Node* parseType(bool IsTemplate = false);
	Node* parseTypeBracket();

	Node* parseIdeitfierScope();
	Node* parseScope();

	bool static IsBinaryOperator(TokenKind Kind) {
		switch (Kind)
		{
			// Арифметические
		case TokenKind::Minus:
		case TokenKind::Plus:
		case TokenKind::Asterisk:
		case TokenKind::Slash:
		case TokenKind::Percent:
			// Операции сравнения
		case TokenKind::Equal:
		case TokenKind::NotEqual:
		case TokenKind::Less:
		case TokenKind::Greater:
		case TokenKind::LessEqual:
		case TokenKind::GreaterEqual:
			// <=>	Трёхстороннее сравнение
			// Побитовые (для целых чисел)
		case TokenKind::Ampersand:
		case TokenKind::Pipe:
		case TokenKind::Caret:
		case TokenKind::Shl:
		case TokenKind::Shr:
			// Логические
		case TokenKind::And:
		case TokenKind::Or:
			//Операции присваивания
		case TokenKind::Assign:
		case TokenKind::PlusAssign:
		case TokenKind::MinusAssign:
		case TokenKind::MultAssign:
		case TokenKind::DivAssign:
		case TokenKind::ModAssign:
		case TokenKind::AndAssign:
		case TokenKind::OrAssign:
		case TokenKind::XorAssign:
		case TokenKind::ShlAssign:
		case TokenKind::ShrAssign:
			return true;
		}
		return false;
	}

	bool static IsUnaryOperator(TokenKind Kind) {
		switch (Kind)
		{
		case TokenKind::Minus:
		case TokenKind::Plus:
		case TokenKind::Dec:
		case TokenKind::Inc:
		case TokenKind::Asterisk:
		case TokenKind::Exclamation:
		case TokenKind::Tilde:
			return true;
		}
		return false;
	}

	// Для бинарных операторов
	int static GetBinaryOperatorPriority(TokenKind Kind) {
		switch (Kind) {
			// Уровень 1: Присваивание (самый низкий приоритет)
		case TokenKind::Assign:
		case TokenKind::PlusAssign:
		case TokenKind::MinusAssign:
		case TokenKind::MultAssign:
		case TokenKind::DivAssign:
		case TokenKind::ModAssign:
		case TokenKind::AndAssign:
		case TokenKind::OrAssign:
		case TokenKind::XorAssign:
		case TokenKind::ShlAssign:
		case TokenKind::ShrAssign:
			return 1;

			// Уровень 2: Логическое ИЛИ
		case TokenKind::Or:
			return 2;

			// Уровень 3: Логическое И
		case TokenKind::And:
			return 3;

			// Уровень 4: Побитовое ИЛИ
		case TokenKind::Pipe:
			return 4;

			// Уровень 5: Побитовое XOR
		case TokenKind::Caret:
			return 5;

			// Уровень 6: Побитовое И
		case TokenKind::Ampersand:
			return 6;

			// Уровень 7: Равенство
		case TokenKind::Equal:
		case TokenKind::NotEqual:
			return 7;

			// Уровень 8: Сравнения
		case TokenKind::Less:
		case TokenKind::Greater:
			// ...
			return 8;

			// Уровень 9: Сдвиги
		case TokenKind::Shl:
		case TokenKind::Shr:
			return 9;

			// Уровень 10: Сложение/вычитание
		case TokenKind::Plus:
		case TokenKind::Minus:
			return 10;

			// Уровень 11: Умножение/деление
		case TokenKind::Asterisk:
		case TokenKind::Slash:
		case TokenKind::Percent:
			return 11;

		default:
			return 0; // Нет приоритета
		}
	}

	
	/*
		// Для унарных операторов
	int static GetUnaryOperatorPriority(TokenKind Kind) {
		switch (Kind) {
			// Уровень 3: Унарные операции
		case TokenKind::Plus:        // унарный плюс
		case TokenKind::Minus:       // унарный минус
		case TokenKind::Inc:         // префиксный инкремент
		case TokenKind::Dec:         // префиксный декремент
		case TokenKind::Exclamation: // логическое НЕ
		case TokenKind::Tilde:       // побитовое НЕ
		case TokenKind::Asterisk:    // разыменование
		case TokenKind::Ampersand:   // взятие адреса
			return 3;

		default:
			return 0;
		}
	}
	*/

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
	case TokenKind::Struct:    return parseStruct();
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

Node* Parser::parseProperty() {
	stream.consume(TokenKind::Property); // __property
	
	// тип (int)
	Node* Type = parseTypeBracket();
	if (!Type) throw std::runtime_error("Expected type in __property");

	// имя (Value)
	std::string name = stream.consume(TokenKind::IdentifierLiteral).value;
	
	// {
	if (!stream.match(TokenKind::LeftBrace))
		throw std::runtime_error("Expected '{' in __property");

	Node *getter = nullptr, *setter = nullptr;
	while (!stream.match(TokenKind::RightBrace)) {
		if (stream.match(TokenKind::Read)) {
			if (!stream.match(TokenKind::Equals))
				throw std::runtime_error("Expected '=' after 'read'");
			getter = parseIdeitfierScope(); // __getValue
		}
		else if (stream.match(TokenKind::Write)) {
			if (!stream.match(TokenKind::Equals))
				throw std::runtime_error("Expected '=' after 'write'");
			setter = parseIdeitfierScope(); // __setValue
		}
		else {
			stream.consume(stream.peek().type);
		}
	}
	
	return new NodeProperty(name, Type, getter, setter);
}

Node* Parser::parseIdeitfierScope() {
	std::string Identifier = "";
	std::vector<std::string> Scope;
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
		default:
			return new NodeIdentifier(Identifier, new NodeScope(Scope));
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

Node* Parser::parseType(bool IsTemplate) {

	/*
	Допустимые вариации типов
	T&          // изменяемая ссылка
	const T&    // неизменяемая ссылка
	T*          // указатель
	const T*    // указатель на константу
	T&&         // rvalue-ссылка (move)
	const T&&   // - бессмысленно, но для простоты парсинга
	*/

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

	std::vector<Node*> TemplateArgs;

	if (stream.peek().type == TokenKind::Less)
	{
		TemplateArgs = parseTemplateList();
		if (stream.peek().type != TokenKind::Greater)
			throw std::runtime_error("Expected Greater token");
		stream.consume(TokenKind::Greater);
	}
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
	case TokenKind::BitAnd:
		stream.consume(TokenKind::BitAnd);
		eType = NodeType::EType::RVALUE;
		break;
	default:
		break;
	}
	return new NodeType(Type, IsTemplate, IsConst, eType, TemplateArgs);
};

std::vector<Node*> Parser::parseTemplateList() {

	stream.consume(TokenKind::Less);

	std::vector<Node*> TemplateList;
	if (stream.peek().type == TokenKind::Greater)
		return TemplateList;

	TemplateList.push_back(parseTemplate());
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		TemplateList.push_back(parseTemplate());
	}
	return TemplateList;
};

Node* Parser::parseTemplate() {
	switch (stream.peek().type)
	{
	case TokenKind::NullptrLiteral:
		return parseNullptr();
	case TokenKind::IdentifierLiteral:
		return parseType(true);
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
	throw std::runtime_error("Not corrected token");
};

Node* Parser::parseTypeBracket() {

	if (!stream.match(TokenKind::LeftBracket))
		throw std::runtime_error("Expected LeftBracket token");

	Node* Type = parseType();

	if (!stream.match(TokenKind::RightBracket))
		throw std::runtime_error("Expected RightBracket token");

	return Type;
};

Node* Parser::parseVar() {

	stream.consume(TokenKind::Var);

	auto Type = parseTypeBracket();

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

	if (IsUnaryOperator(stream.peek().type))
	{
		UnaryOp = getUnaryOperand(stream.peek().type);
		stream.consume(stream.peek().type);
	}

	Node* Right = nullptr;

	switch (stream.peek().type) {
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
	default: 
	{
		// throw std::runtime_error("Unexpected token in primary expression");
	}
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
		if (!IsBinaryOperator(op))
			break;
		int currentPriority = GetBinaryOperatorPriority(op);
		if (currentPriority < MinPrec)
			break;
		stream.consume(op);
		Node* Right = parseExpression(currentPriority + 1);
		Left = new NodeBinaryOp(getBinaryOperand(op), Left, Right);
	}

	return Left;
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

Node* Parser::parseNodeCall(Node* Func) {

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

	if (stream.peek().type != TokenKind::Var)
		return nullptr;
	stream.consume(TokenKind::Var);

	auto Type = parseTypeBracket();
	if (!Type) return nullptr;

	std::vector<Node*> ContainerDeclarationList;
	ContainerDeclarationList.push_back(parseDeclaration());
	return new NodeDeclarationList(Type, ContainerDeclarationList);
}

std::vector<Node*> Parser::parseArgumentList() {
	std::vector<Node*> ArgumentList;
	Node* OneArgument = parseArgument();
	if (!OneArgument) return ArgumentList;
	ArgumentList.push_back(OneArgument);
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		ArgumentList.push_back(parseArgument());
	}
	return ArgumentList;
}

Node* Parser::parseFunction() {

	stream.consume(TokenKind::Function);

	auto Type = parseTypeBracket();

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
	
	Node* FunctionName = parseIdeitfierScope();
	
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

	auto Type = parseTypeBracket();

	if (!Type)
		return nullptr;

	// Имя лямбды
	if (stream.peek().type != TokenKind::IdentifierLiteral) {
		return nullptr;
	}
	Node* LambdaName = parseIdeitfierScope();

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

	Node* Scope = parseScope();

	if (stream.peek().type != TokenKind::Semicolon)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Semicolon);

	// Temporary stub
	return new NodeAccess(Name, Scope);
};

Node* Parser::parseUsing() {
	
	stream.consume(TokenKind::Using);

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected IdentifierLiteral token");

	std::string Name = stream.consume(TokenKind::IdentifierLiteral).value;

	if (stream.peek().type != TokenKind::Equals)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Equals);

	Node* Path = parseIdeitfierScope();

	if (stream.peek().type != TokenKind::Semicolon)
		throw std::runtime_error("Expected Equals token");
	stream.consume(TokenKind::Semicolon);

	// Temporary stub
	return new NodeUsing(Name, Path);
};

Node* Parser::parseGenericParametr() {

	Node* genericParams = nullptr;

	if (stream.peek().type != TokenKind::IdentifierLiteral)
		throw std::runtime_error("Expected generic parameter name");
	Node* paramName = parseIdeitfierScope();

	Node* defaultExpr = nullptr;

	if (stream.match(TokenKind::Equals)) {
		if (stream.peek().type == TokenKind::LeftBracket)
		{
			defaultExpr = parseTypeBracket();
		}
		if (stream.peek().type == TokenKind::IdentifierLiteral)
		{
			defaultExpr = parseIdentifier();
		}
	}
	return new NodeDeclaration(paramName, defaultExpr);
}

Node* Parser::parseGenericParametrs() {

	std::vector<Node*> genericParam;
	genericParam.push_back(parseGenericParametr());
	while (stream.peek().type == TokenKind::Comma) {
		stream.consume(TokenKind::Comma);
		genericParam.push_back(parseGenericParametr());
	}
	return new NodeGenericParams(genericParam);
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
				genericParamsConcretic->add(parseIdeitfierScope());
			}
			else
			{
				// Не заморачиваемся с выражениями
				genericParamsConcretic->add(new NodeIdentifier(stream.consume(stream.peek().type).value, {}));
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

	Node* genericParams = nullptr;

	// Generic-параметры: [T, K = [int]]
	if (stream.match(TokenKind::LeftBracket))
	{
		if (stream.peek().type != TokenKind::RightBracket)
			genericParams = parseGenericParametrs();
		if (stream.peek().type != TokenKind::RightBracket)
			throw std::runtime_error("Expected RightBracket token");
		stream.consume(TokenKind::RightBracket);
	}

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
		throw std::runtime_error("Expected struct name");

	Node* genericParams = nullptr;

	// Generic-параметры: [T, K = [int]]
	if (stream.match(TokenKind::LeftBracket))
	{
		if (stream.peek().type != TokenKind::RightBracket)
			genericParams = parseGenericParametrs();
		if (stream.peek().type != TokenKind::RightBracket)
			throw std::runtime_error("Expected RightBracket token");
		stream.consume(TokenKind::RightBracket);
	}

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