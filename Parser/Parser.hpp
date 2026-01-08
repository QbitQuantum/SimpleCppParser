
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
	struct StructTypeQualifier {
		std::string Type = "";
		NodeTypeQualifier::Qualifers Qualifer;
	};


	std::unordered_map<std::string, StructTypeQualifier> ResolvedAliasType;

	std::unordered_map<TTokenID, ParserEnginePtr> map{{
	{TTokenID::Var, &Parser::Var},
	{TTokenID::Using, &Parser::Using},
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

	Node* ResolvingType();
	Node* ResolvingAccess();
	Node* ResolvingPointer();

	Node* Var();
	Node* Using();
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
	NodeTypeQualifier::Qualifers Qualifer;

	if (NextToken() && !match(TTokenID::LeftBracket))
		return nullptr;
	
	while (NextToken() && !match(TTokenID::RightBracket))
	{
		switch (GetToken().type)
		{
		case TTokenID::Const: Qualifer.IsConst = true;
			break;
		case TTokenID::Asterisk: Qualifer.IsRef = true;
			break;
		case TTokenID::ScResOp: Type = "";
			break;
		case TTokenID::IdentifierLiteral:
			Type = GetToken().value;
			break;
		case TTokenID::Type: 
		{
			NextToken();
			NextToken();
			std::string search = GetToken().value;
			NextToken();
			auto it = ResolvedAliasType.find(search);
			if (it != ResolvedAliasType.end())
			{
				auto Data = ResolvedAliasType[search];
				Type = Data.Type;
				Qualifer.IsConst = Data.Qualifer.IsConst;
				Qualifer.IsRef = Data.Qualifer.IsRef;
			}
			break;
		}
		case TTokenID::Access:
		{
			NextToken();
			NextToken();
			std::string search = GetToken().value;
			NextToken();
			break;
		}
		}

	}
	return new NodeTypeQualifier(Type, Qualifer);
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

Node* Parser::Using() {

	if (!NextToken())
		return nullptr;

	switch (GetToken().type)
	{
	case TTokenID::Type:
		return ResolvingType();
	case TTokenID::Access:
		return ResolvingAccess();
	case TTokenID::Pointer:
		return ResolvingPointer();
	default:
		return nullptr;
	}
	return nullptr;
};

Node* Parser::Function() {

	if (NextToken() && !match(TTokenID::LeftBracket))
		return nullptr;

	std::string Type = "";
	NodeFunction::Qualifers Qualifer;

	while (NextToken() && !match(TTokenID::RightBracket))
	{
		switch (GetToken().type)
		{
		case TTokenID::Const: Qualifer.IsConst = true;
			break;
		case TTokenID::Asterisk: Qualifer.IsRef = true;
			break;
		case TTokenID::ScResOp: Type = "";
			break;
		case TTokenID::IdentifierLiteral:
			Type = GetToken().value;
			break;
		case TTokenID::Type:
		{
			NextToken();
			NextToken();
			std::string search = GetToken().value;
			NextToken();
			NextToken();
			auto it = ResolvedAliasType.find(search);
			if (it != ResolvedAliasType.end())
			{
				auto Data = ResolvedAliasType[search];
				Type = Data.Type;
				Qualifer.IsConst = Data.Qualifer.IsConst;
				Qualifer.IsRef = Data.Qualifer.IsRef;
			}
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
	return new NodeFunction(Type, Qualifer, FunctiomName, ArgumentList);
};

Node* Parser::ResolvingType() {

	NextToken();

	std::string name = GetToken().value;

	if (NextToken() && !match(TTokenID::Equals))
		return nullptr;

	StructTypeQualifier structTypeQualifier;

	while (!match(TTokenID::Semicolon) && NextToken())
	{

		switch (GetToken().type)
		{
		case TTokenID::Const: structTypeQualifier.Qualifer.IsConst = true;
			break;
		case TTokenID::Asterisk: structTypeQualifier.Qualifer.IsRef = true;
			break;
		case TTokenID::IdentifierLiteral:
			structTypeQualifier.Type = GetToken().value;

			break;
		case TTokenID::ScResOp:
			structTypeQualifier.Type = "";
			break;
		}
	}
	ResolvedAliasType[name] = structTypeQualifier;
	return new NodeUsingType(name, new NodeTypeQualifier(structTypeQualifier.Type, structTypeQualifier.Qualifer));
};
Node* Parser::ResolvingAccess() {
	while (!match(TTokenID::Semicolon) && NextToken()); { }
	return new NodeUsingAcess();
};

Node* Parser::ResolvingPointer() {
	return nullptr;
};


Parser::~Parser()
{
	for (auto& i : ast)
		if (i) delete i;
}

#endif // PARSER_HPP