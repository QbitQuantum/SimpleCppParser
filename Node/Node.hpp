
#ifndef NODE_HPP
#define NODE_HPP
#pragma once

#include <string>
#include <memory>

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
    std::string Type = "";
    bool IsConst = false;
    bool IsRef = false;
public:
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
    CType* Qualifer = nullptr;
public:
    std::string print() override {
        std::string fprint = Qualifer ? 
            "[" + Qualifer->print() + "]" : "";
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
        std::string fprint = "var";
        if (TypeQualifier)
        {
            int size = DeclarationList.size();
            fprint += TypeQualifier->print();
            for (size_t i = 0; i < size; i++)
                if (auto Decl = DeclarationList[i]; Decl)
                {
                    std::string DeclName = Decl->print();
                    std::string separator = (i == size - 1 ? "" : ", ");
                    fprint += (DeclName.empty() ? "" : " ") + DeclName + separator;
                }
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
        return "using " + Name + " = " + Qualifer->print();
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
    NodeTypeQualifier* TypeQualifier = nullptr;
    std::string Name = "";
    std::vector<NodeDeclarationList*> ArgumentList;
    Node* Body = nullptr;
public:
    NodeFunction(
        NodeTypeQualifier* typequalifer, std::string name, const std::vector<NodeDeclarationList*> argumentList, Node* body = nullptr) :
        TypeQualifier(typequalifer), Name(name),  ArgumentList(argumentList), Body(body) { };

    std::string print() override {  
        std::string fprint = "function "  + TypeQualifier->print() + " " + Name;
        
        fprint += "(";
        int size = ArgumentList.size();
        for (size_t i = 0; i < size; i++)
            if (auto Decl = ArgumentList[i]; Decl)
                fprint += Decl->print() + (i == size - 1 ? "" : ", ");

        fprint += ")";

        if (Body) {
            fprint += " " + Body->print();
        }

        return fprint;
    };

    ~NodeFunction() {
        delete TypeQualifier; TypeQualifier = nullptr;
        for (auto& i : ArgumentList) delete i;
        delete Body; Body = nullptr;
    };
};


class NodeBlock : public Node
{
    std::vector<Node*> Statements;
public:
    NodeBlock() = default;

    void add(Node* stmt) {
        if (stmt) Statements.push_back(stmt);
    }

    std::string print() override {
        std::string fprint = "{ ";
        int size = static_cast<int>(Statements.size());
        for (int i = 0; i < size; ++i) {
            if (auto stmt = Statements[i]) {
                fprint += stmt->print();
                if (i != size - 1)
                    fprint += "; ";
            }
        }
        fprint += " }";
        return fprint;
    }

    ~NodeBlock() override {
        for (auto* stmt : Statements) {
            delete stmt;
        }
    }
};

class NodeClass : public Node
{
    std::string Name;
    std::string BaseClass;
    NodeBlock* Body = nullptr;
public:
    NodeClass(const std::string& name, const std::string& baseClass, NodeBlock* body) :
        Name(name), BaseClass(baseClass), Body(body) {
    }

    std::string print() override {
        std::string fprint = "class " + Name;
        if (!BaseClass.empty()) {
            fprint += " : " + BaseClass;
        }
        if (Body) {
            fprint += " " + Body->print();
        }
        return fprint;
    }

    ~NodeClass() override {
        delete Body; Body = nullptr;
    }
};

struct NumberNode : Node {
    double value;
    NumberNode(double v) : value(v) {}

    std::string print() override {
        return "";
    }
};



struct BinaryOpNode : Node {
    std::string op;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    BinaryOpNode(const std::string& o, std::unique_ptr<Node> l,
        std::unique_ptr<Node> r)
        : op(o), left(std::move(l)), right(std::move(r)) {
    }

    std::string print() override {
        return "";
    }
};

struct CallNode : Node {
    std::string funcName;
    std::vector<std::unique_ptr<Node>> args;

    CallNode(const std::string& name) : funcName(name) {}

    void addArg(std::unique_ptr<Node> arg) {
        args.push_back(std::move(arg));
    }

    std::string print() override {
        return "";
    }
};


#endif // NODE_HPP