
#ifndef PARSER_HPP
#define PARSER_HPP
#pragma once

#include "PostLexer.hpp"
#include "Node.hpp"

#include <vector>
#include <unordered_map>

class Parser
{
	using ParserEnginePtr = Node * (Parser::*)();
private:
	int PosBuffer = 0;
	int SizeBufferParser = 0;

	std::vector<Node*> ast;
	void Init();

	std::unordered_map<std::string, CType> ResolvedAliasType;

	std::unordered_map<TTokenID, ParserEnginePtr> map{{
	{TTokenID::Access, &Parser::Access},
	{TTokenID::Alias, &Parser::Alias},
	{TTokenID::Pointer, &Parser::Pointer},
	{TTokenID::Var, &Parser::Var},
	{TTokenID::Function, &Parser::Function},
	}};

	LexToken GetToken () {
		return ParserEngineBuffer[PosBuffer];
		};

	bool match(TTokenID ID) {
		return GetToken().type == ID;
		};

	bool neof() {
		return PosBuffer < SizeBufferParser;
	}

	bool NextToken() {
		PosBuffer++;
		while (match(TTokenID::LineFeed) || match(TTokenID::Space))
			PosBuffer++;
		return neof();
		};

	Node* Access();
	Node* Alias();
	Node* Pointer();
	Node* Var();
	Node* Function();

	NodeTypeQualifier* TypeQualifierParse();
public:
	std::vector<LexToken> ParserEngineBuffer;

	Parser(const PostLexer& advance) : 
		ParserEngineBuffer(advance.GetBufferPostLexerToken()) {
		Init();
	};

	Parser(const std::vector<LexToken>& Buffer) : 
		ParserEngineBuffer(Buffer) {
		Init();
	};

	~Parser();

private:

};

void Parser::Init() {
	SizeBufferParser = ParserEngineBuffer.size();
	
	while (neof()) {
		if (auto it = map.find(ParserEngineBuffer[PosBuffer].type); 
		it != map.end()) ast.push_back((this->*it->second)());
		PosBuffer++;
	}

#ifdef _IOSTREAM_
	if (false)
		for (auto& i : ast)
			if (i) std::cout << i->print() << "\n";
#endif // _IOSTREAM_

}

NodeTypeQualifier* Parser::TypeQualifierParse() {
	std::string Type = "";
	bool IsConst = false;
	bool IsRef = false;
	std::string ResolvingAlias = "";

	if (NextToken() && !match(TTokenID::LeftBracket))
		return nullptr;
	
	while (NextToken() && !match(TTokenID::RightBracket))
	{
		switch (GetToken().type)
		{
		case TTokenID::Const: IsConst = true;
			break;
		case TTokenID::Asterisk: IsRef = true;
			break;
		case TTokenID::ScResOp: Type = "";
			break;
		case TTokenID::IdentifierLiteral:
			Type = GetToken().value;
			break;
		case TTokenID::Alias:
		{
			NextToken();
			NextToken();
			ResolvingAlias = GetToken().value;
			NextToken();
			break;
		}
		case TTokenID::Access:
		{
			NextToken();
			NextToken();
			std::string Resolving = GetToken().value;
			NextToken();
			break;
		}
		}
	}

	CType* cType = nullptr;

	if (auto it = ResolvedAliasType.find(ResolvingAlias); it != ResolvedAliasType.end())
		cType = new CType(ResolvedAliasType[ResolvingAlias]);

	return new NodeTypeQualifier(cType ? cType : new CType(Type, IsConst, IsRef));
};

Node* Parser::Var() {

	auto TypeQualifier = TypeQualifierParse();

	std::vector<NodeDeclaration*> ContainerDeclarationList;

	auto ParseInitializer = [&]() -> std::string {
		
		std::string initializer = "";

		while (NextToken() && !match(TTokenID::Semicolon) && !match(TTokenID::Comma))
		{
			switch (GetToken().type)
			{
			case TTokenID::Access:
			{
				NextToken();
				NextToken();
				std::string search = GetToken().value;
				NextToken();
				break;
			}
			case TTokenID::IdentifierLiteral:
			{
				initializer = GetToken().value;
				break;
			}
			case TTokenID::ScResOp:
			{
				initializer = "";
				break;
			}
			default:
				initializer = GetToken().value;
				break;
			}
		}
		return initializer;
		};

	while (NextToken() && !match(TTokenID::Semicolon))
	{
		if (!match(TTokenID::IdentifierLiteral))
			break;

		std::string Initializer = "", Name = GetToken().value;
		if (NextToken() && match(TTokenID::Equals))
			Initializer = ParseInitializer();
		
		ContainerDeclarationList.push_back(
			new NodeDeclaration(new NodeIdentifier(Name),
				Initializer.empty() ? nullptr :
				new NodeIdentifier(Initializer)));

		if (match(TTokenID::Semicolon))
			break;
	}
	return new NodeDeclarationList(TypeQualifier, ContainerDeclarationList);

};

Node* Parser::Function() {

	if (NextToken() && !match(TTokenID::LeftBracket))
		return nullptr;

	std::string Type = "";
	bool IsConst = false;
	bool IsRef = false;
	std::string ResolvingAlias = "";

	while (NextToken() && !match(TTokenID::RightBracket))
	{
		switch (GetToken().type)
		{
		case TTokenID::Const: IsConst = true;
			break;
		case TTokenID::Asterisk: IsRef = true;
			break;
		case TTokenID::ScResOp: Type = "";
			break;
		case TTokenID::IdentifierLiteral:
			Type = GetToken().value;
			break;
		case TTokenID::Alias:
		{
			NextToken();
			NextToken();
			std::string ResolvingAlias = GetToken().value;
			NextToken();
			NextToken();
			break;
		}
		case TTokenID::Access:
		{
			NextToken();
			NextToken();
			std::string search = GetToken().value;
			NextToken();
			NextToken();
			break;
		}
		}
	}

	if (NextToken() && !match(TTokenID::LeftBracket))
		return nullptr;

	while (!match(TTokenID::RightBracket) && NextToken()); // Парсинг квалификаторов

	if (NextToken() && !match(TTokenID::IdentifierLiteral))
		return nullptr;

	std::string FunctiomName = GetToken().value;

	std::vector<NodeDeclarationList*> ArgumentList;
	if (NextToken() && !match(TTokenID::LeftParen))
		return nullptr;

	auto ParseInitializer = [&]() -> std::string {

		std::string initializer = "";

		while (NextToken() && !match(TTokenID::RightParen) && !match(TTokenID::Comma))
		{
			switch (GetToken().type)
			{
			case TTokenID::Access:
			{
				NextToken();
				NextToken();
				std::string search = GetToken().value;
				NextToken();
				break;
			}
			case TTokenID::IdentifierLiteral:
			{
				initializer = GetToken().value;
				break;
			}
			case TTokenID::ScResOp:
			{
				initializer = "";
				break;
			}
			default:
				initializer = GetToken().value;
				break;
			}
		}

		return initializer;
		};

	while (NextToken() && !match(TTokenID::RightParen))
	{
		if (!match(TTokenID::Var))
			continue;

		auto TypeQualifier = TypeQualifierParse();

		std::string Name = "", Initializer = "";
		if (NextToken())
		{

			if (match(TTokenID::IdentifierLiteral))
			{
				Name = GetToken().value;
				if (NextToken() && match(TTokenID::Equals))
					Initializer = ParseInitializer();
			}
			if (match(TTokenID::Equals))
				Initializer = ParseInitializer();
		}
		
		ArgumentList.push_back(new NodeDeclarationList(TypeQualifier, Name.empty() ? std::vector<NodeDeclaration*>{} :
			std::vector<NodeDeclaration*>{ new NodeDeclaration(new NodeIdentifier(Name),
				Initializer.empty() ? nullptr :
				new NodeIdentifier(Initializer)) }));

		if (match(TTokenID::RightParen))
			break;
	}

	CType* cType = nullptr;

	if (auto it = ResolvedAliasType.find(ResolvingAlias); it != ResolvedAliasType.end())
		cType = new CType(ResolvedAliasType[ResolvingAlias]);

	return new NodeFunction(cType ? cType : new CType(Type, IsConst, IsRef), FunctiomName, ArgumentList);
};

Node* Parser::Access() {
	while (NextToken() && !match(TTokenID::Semicolon)); { }
	return new NodeAccess();
};

Node* Parser::Alias() {

	if (NextToken() && !match(TTokenID::IdentifierLiteral))
		return nullptr;

	std::string name = GetToken().value;

	if (NextToken() && !match(TTokenID::Equals))
		return nullptr;

	std::string Type = "";
	bool IsConst = false;
	bool IsRef = false;

	while (NextToken() && !match(TTokenID::Semicolon))
	{
		switch (GetToken().type)
		{
		case TTokenID::Const: IsConst = true;
			break;
		case TTokenID::Asterisk: IsRef = true;
			break;
		case TTokenID::IdentifierLiteral:
			Type = GetToken().value;

			break;
		case TTokenID::ScResOp:
			Type = "";
			break;
		}
	}

	ResolvedAliasType[name] = CType(Type, IsConst, IsRef);
	return new NodeAlias(name, new CType(Type, IsConst, IsRef));
};

Node* Parser::Pointer() {
	return nullptr;
};


Parser::~Parser()
{
	for (auto& i : ast)
		if (i) delete i;
}

#endif // PARSER_HPP