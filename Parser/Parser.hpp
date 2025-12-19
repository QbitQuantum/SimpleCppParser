
#ifndef PARSER_HPP
#define PARSER_HPP
#pragma once

#include "PostLexer.hpp"
#include "Node.hpp"

#include <vector>
#include <unordered_map>

class Parser
{
	struct VariableDeclaration
	{
		std::string Name;
		std::string Initializer;
	};

	using ParserEnginePtr = Node * (Parser::*)();
private:
	int PosBuffer = 0;
	int SizeBufferParser = 0;

	std::vector<Node*> ast;
	void Init();
	bool neof() {
		return PosBuffer < SizeBufferParser;
	}

	std::unordered_map<TTokenID, ParserEnginePtr> map{{
	{TTokenID::Var, &Parser::Var},
	}};

	Node* Var();

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

Node* Parser::Var() {

	auto GetToken = [&]() -> LexToken {
		return ParserEngineBuffer[PosBuffer];
		};

	auto match = [&](TTokenID ID) -> bool {
		return GetToken().type == ID;
		};

	auto NextToken = [&]()-> bool {
		PosBuffer++;
		while (match(TTokenID::LineFeed) || match(TTokenID::Space))
			PosBuffer++;
		return true;
		};

	
	std::string Type = "";
	NodeTypeQualifier::Qualifers Qualifer;
	std::vector<VariableDeclaration> Declarations;

	if (NextToken() && !match(TTokenID::LeftBracket) && NextToken())
		return nullptr;
	
	while (!match(TTokenID::RightBracket) && NextToken())
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
		}
	}

	bool IsFindEquals = false;

	auto parseQualifiedName = [&]() -> std::string {
		
		std::string initializer = GetToken().value;
		
		if (!match(TTokenID::IdentifierLiteral))
			return GetToken().value;

		while (!match(TTokenID::Semicolon) && !match(TTokenID::Comma) && NextToken())
		{
			switch (GetToken().type)
			{
			case TTokenID::ScResOp: initializer = "";
				break;
			case TTokenID::IdentifierLiteral:
				initializer = GetToken().value;
				break;
			}
		}
		return initializer;
		};

	while (!match(TTokenID::Semicolon) && NextToken())
	{
		if (!match(TTokenID::IdentifierLiteral))
			break;
		VariableDeclaration Decl;
		Decl.Name = GetToken().value;
		if (NextToken() && match(TTokenID::Equals) && NextToken())
			Decl.Initializer = parseQualifiedName();
		Declarations.push_back(Decl);
	}

	NodeDeclarationList* DeclarationList = nullptr;
	NodeTypeQualifier* TypeQualifier = new NodeTypeQualifier(Type, Qualifer);
	std::vector<NodeDeclaration*> ContainerDeclarationList;
	for (int i = 0; i < Declarations.size(); i++)
		ContainerDeclarationList.push_back(
			new NodeDeclaration(new NodeIdentifier(Declarations[i].Name),
				Declarations[i].Initializer.empty() ? nullptr :
				new NodeIdentifier(Declarations[i].Initializer)));
	DeclarationList = new NodeDeclarationList(TypeQualifier, ContainerDeclarationList);
	return DeclarationList;

};

Parser::~Parser()
{
	for (auto& i : ast)
		if (i) delete i;
}

#endif // PARSER_HPP