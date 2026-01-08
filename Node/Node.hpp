
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
        if (TypeQualifier)
        {
            int size = DeclarationList.size();
            fprint += TypeQualifier->print() + (size ? " " : "");
            for (size_t i = 0; i < size; i++)
                if (auto Decl = DeclarationList[i]; Decl)
                    fprint += Decl->print() + (i == size - 1 ? "" : ", ");
        }
        return fprint;
    };
    NodeDeclarationList(NodeTypeQualifier* typeQualifier, const std::vector<NodeDeclaration*>& declarationList) :
        TypeQualifier(typeQualifier), DeclarationList(declarationList) { };

    ~NodeDeclarationList() override {
        delete TypeQualifier;
        for (auto& decl : DeclarationList) {
            delete decl;
        }
    };
};

class NodeAlias : public Node
{
    std::string Name = "";
    std::string Type = "";
public:
    struct Qualifers
    {
        bool IsConst = false;
        bool IsRef = false;
    } Qualifer;
private:
public:
    NodeAlias(std::string name, std::string type, Qualifers qualifer) :
        Name(name), Type(type), Qualifer(qualifer) {
    };
    std::string print() override {
        return "alias " + Name + " = " +
            (Qualifer.IsConst ? std::string("const ") : std::string("")) +
            Type +
            (Qualifer.IsRef ? std::string("*") : std::string(""));
    };
    ~NodeAlias() { }
};

class NodeAccess : public Node
{
public:
    NodeAccess() {};
    std::string print() override { return ""; };
    ~NodeAccess() {};
};

class NodeFunction : public Node
{
    
    std::string Type = "";
public:
    struct Qualifers
    {
        bool IsConst = false;
        bool IsRef = false;
    } Qualifer;
private:
    std::string Name = "";

    std::vector<NodeDeclarationList*> ArgumentList;

public:
    NodeFunction(
        std::string type, Qualifers qualifer, std::string name, const std::vector<NodeDeclarationList*>& argumentList) :
        Type(type), Qualifer(qualifer), Name(name),  ArgumentList(argumentList) { };

    std::string print() override {  
        std::string fprint = "function ["  +
        (Qualifer.IsConst ? std::string("const ") : std::string("")) +
        Type +
        (Qualifer.IsRef ? std::string("*") : std::string("")) + "][__fastcall] " + Name;
        
        fprint += "(";
        int size = ArgumentList.size();
        for (size_t i = 0; i < size; i++)
            if (auto Decl = ArgumentList[i]; Decl)
                fprint += Decl->print() + (i == size - 1 ? "" : ", ");

        fprint += ")";

        return fprint;
    };

    ~NodeFunction() {
        for (auto& i : ArgumentList) delete i;
    
    };
};

#endif // NODE_HPP