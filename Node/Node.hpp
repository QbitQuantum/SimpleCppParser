
#ifndef NODE_HPP
#define NODE_HPP
#pragma once

#include <string>

class Node
{
protected:
    int Type = -1;
    Node() {};
public:
    virtual std::string print() = 0;
    virtual ~Node() = default;
};

class NodeTypeQualifier : public Node
{
public:
    std::string Type;

    struct Qualifers
    {
        bool IsConst = false;
        bool IsRef = false;
    } Qualifer;

    std::string print() override {
        std::string fprint = "";
        if (Type.empty())
            return "Type.empty()";
        fprint = 
            "var[" + 
            (Qualifer.IsConst ? std::string("const ") : std::string("")) +
            Type + 
            (Qualifer.IsRef ? std::string("*") : std::string(""))
            + "]";
        return fprint;
    };

    NodeTypeQualifier(const std::string type, const Qualifers& qualifer) :
        Type(type), Qualifer(qualifer) {};

    ~NodeTypeQualifier() override {};
};

class NodeIdentifier : public Node
{
    std::string Name = "";
public:
    NodeIdentifier(const std::string& name) :
        Name(name) {
    };
    std::string print() override { return Name; };
    ~NodeIdentifier() override {
    };
};
class NodeDeclaration : public Node
{
    NodeIdentifier* Identifier = nullptr;
    Node* Initializer = nullptr;
public:
    std::string print() override {
        std::string fprint = Identifier->print();
        if (Initializer)
            fprint += " = " + Initializer->print();
        return fprint;
    };
    NodeDeclaration(NodeIdentifier* identifier, Node* initializer) :
        Identifier(identifier), Initializer(initializer) {
    };

    ~NodeDeclaration() override {
        delete Identifier; Identifier = nullptr;
        delete Initializer; Initializer = nullptr;
    };
};

class NodeDeclarationList : public Node
{
    NodeTypeQualifier* TypeQualifier = nullptr;
    std::vector<NodeDeclaration*> DeclarationList;
public:
    std::string print() override {
        std::string fprint = "";
        if (!TypeQualifier)
            return "!TypeQualifier";
        fprint += TypeQualifier->print() + " ";
        for (auto& Decl : DeclarationList)
            if (Decl)
                fprint += Decl->print() + ", ";
        fprint.pop_back();
        fprint.pop_back();
        return fprint;
    };
    NodeDeclarationList(NodeTypeQualifier*& typeQualifier, const std::vector<NodeDeclaration*>& declarationList) :
        TypeQualifier(typeQualifier), DeclarationList(declarationList) { };

    ~NodeDeclarationList() override {
        delete TypeQualifier;
        for (auto& decl : DeclarationList) {
            delete decl;
        }
    };
};

#endif // NODE_HPP