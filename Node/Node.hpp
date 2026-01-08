
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

// TODO: Этот класс имеет фукцию print, что по теории моей шизы должен быть частью Node
class CType {
    
public:
    std::string Type = "";
    bool IsConst = false;
    bool IsRef = false;
    CType(std::string type, bool isConst, bool isRef) :
        Type(type), IsConst(isConst), IsRef(isRef) {};
    
    CType(CType& type) {
        Type = type.Type;
        IsConst = type.IsConst;
        IsRef = type.IsRef;
    };

    CType() {};
    ~CType() {};
    std::string print() {
        std::string fprint  = (IsConst ? std::string("const ") : std::string("")) +
            Type +
            (IsRef ? std::string("*") : std::string(""));
        return fprint;
    };
};

class NodeTypeQualifier : public Node
{
    CType * Qualifer = nullptr;
public:
    std::string print() override {
        std::string fprint = 
            "var[" + Qualifer->print() + "]";
        return fprint;
    };

    NodeTypeQualifier(CType * qualifer) :
        Qualifer(qualifer) {};

    ~NodeTypeQualifier() override {
        delete Qualifer; Qualifer = nullptr;
    };
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
    CType * Qualifer = nullptr;
public:
    NodeAlias(std::string name, CType* qualifer) :
        Name(name), Qualifer(qualifer) {
    };
    std::string print() override {
        return "alias " + Name + " = " + Qualifer->print();
    };
    ~NodeAlias() {
        delete Qualifer; Qualifer = nullptr;
    }
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
    CType  * Qualifer = nullptr;
    std::string Name = "";
    std::vector<NodeDeclarationList*> ArgumentList;
public:
    NodeFunction(
        CType * qualifer, std::string name, const std::vector<NodeDeclarationList*> argumentList) :
        Qualifer(qualifer), Name(name),  ArgumentList(argumentList) { };

    std::string print() override {  
        std::string fprint = "function ["  + Qualifer->print() + "] [__fastcall] " + Name;
        
        fprint += "(";
        int size = ArgumentList.size();
        for (size_t i = 0; i < size; i++)
            if (auto Decl = ArgumentList[i]; Decl)
                fprint += Decl->print() + (i == size - 1 ? "" : ", ");

        fprint += ")";

        return fprint;
    };

    ~NodeFunction() {
        delete Qualifer; Qualifer = nullptr;
        for (auto& i : ArgumentList) delete i;
    
    };
};

#endif // NODE_HPP