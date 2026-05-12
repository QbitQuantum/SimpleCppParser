
#ifndef PARSER_HPP
#define PARSER_HPP
#pragma once

#include "PostLexer.hpp"
#include "Node.hpp"

#include <vector>
#include <unordered_map>

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

	std::unordered_map<std::string, CType> ResolvedAliasType;

	Node* parseTopLevel();
	Node* parseAccess();
	Node* parsePointer();
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
	case TTokenID::Pointer:  return parsePointer();
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
	std::string ResolvingAlias = "";

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

	CType* cType = nullptr;

	if (auto it = ResolvedAliasType.find(ResolvingAlias); it != ResolvedAliasType.end())
		cType = new CType(ResolvedAliasType[ResolvingAlias]);

	return new NodeTypeQualifier(cType ? cType : new CType(Type, IsConst, IsRef));
};

Node* Parser::parseVar() {
	
	stream.consume(TTokenID::Var);

	size_t savedPos = stream.Pos;

	auto TypeQualifier = TypeQualifierParse();
	if (!TypeQualifier) {
		stream.Pos = savedPos;
		return nullptr;
	}

	std::vector<NodeDeclaration*> ContainerDeclarationList;

	auto ParseInitializer = [&]() -> Node* {
		// Парсим инициализатор (может быть выражением, но для простоты - идентификатор или число)
		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			std::string id = stream.consume(TTokenID::IdentifierLiteral).value;
			return new NodeIdentifier(id);
		}
		else if (stream.peek().type == TTokenID::IntegerLiteral) {
			std::string num = stream.consume(TTokenID::IntegerLiteral).value;
			return new NumberNode(std::stod(num));
		}
		else if (stream.peek().type == TTokenID::StringLiteral) {
			stream.consume(TTokenID::StringLiteral);
		}
		return nullptr;
		};

	// Парсим список объявлений: name1, name2 = init, name3
	do {
		// Ожидаем идентификатор
		if (stream.peek().type != TTokenID::IdentifierLiteral) {
			break;
		}

		std::string Name = stream.consume(TTokenID::IdentifierLiteral).value;
		Node* Initializer = nullptr;

		if (stream.match(TTokenID::Equals)) {
			Initializer = ParseInitializer();
		}

		ContainerDeclarationList.push_back(
			new NodeDeclaration(new NodeIdentifier(Name), Initializer)
		);

	} while (stream.match(TTokenID::Comma));

	// Ожидаем точку с запятой
	if (!stream.match(TTokenID::Semicolon)) {
		// Ошибка: ожидалась ';'
		for (auto* decl : ContainerDeclarationList) delete decl;
		delete TypeQualifier;
		return nullptr;
	}

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

	auto ParseInitializer = [&]() -> Node* {
		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			std::string val = stream.consume(TTokenID::IdentifierLiteral).value;
			return new NodeIdentifier(val);
		}
		else if (stream.peek().type == TTokenID::IntegerLiteral) {
			std::string val = stream.consume(TTokenID::IntegerLiteral).value;
			return new NumberNode(std::stod(val));
		}
		return nullptr;
		};

	// Парсим аргументы: var[const int] name = default
	while (!stream.match(TTokenID::RightParen)) {

		if (stream.peek().type != TTokenID::Var) {
			break;
		}
		stream.consume(TTokenID::Var);

		NodeTypeQualifier* ArgQualifier = TypeQualifierParse();
		if (!ArgQualifier) {
			break;
		}

		std::string ArgName = "";
		Node* DefaultValue = nullptr;

		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			ArgName = stream.consume(TTokenID::IdentifierLiteral).value;
		}

		if (stream.match(TTokenID::Equals)) {
			DefaultValue = ParseInitializer();
		}

		std::vector<NodeDeclaration*> Decls;
		if (!ArgName.empty()) {
			Decls.push_back(new NodeDeclaration(new NodeIdentifier(ArgName), DefaultValue));
		}

		ArgumentList.push_back(new NodeDeclarationList(ArgQualifier, Decls));

		if (stream.peek().type != TTokenID::Comma)
			break;
		stream.consume(TTokenID::Comma);
	}

	// То ли костыль, то ли что
	stream.consume(TTokenID::RightParen);

	// Тело функции или ';'
	Node* body = nullptr;

	if (stream.match(TTokenID::LeftBrace)) {
		body = parseBlock();
	}
	else if (stream.match(TTokenID::Semicolon)) {
		// Прототип функции
	}
	else {
		// Ошибка: ожидалось тело или ;
		for (auto* arg : ArgumentList) delete arg;
		return nullptr;
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

Node* Parser::parsePointer() {
	return nullptr;
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
	if (stream.match(TTokenID::LeftBracket)) {
		if (stream.peek().type == TTokenID::IdentifierLiteral) {
			name = stream.consume(TTokenID::IdentifierLiteral).value;
		}
		stream.match(TTokenID::RightBracket);
	}
	else if (stream.peek().type == TTokenID::IdentifierLiteral) {
		name = stream.consume(TTokenID::IdentifierLiteral).value;
	}

	NodeBlock* body = nullptr;
	if (stream.peek().type == TTokenID::LeftBrace) {
		body = parseBlock();
	}

	return new NodeClass(name, body);
}

#endif // PARSER_HPP