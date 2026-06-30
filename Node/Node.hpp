
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
    std::vector<Node*> DeclarationList;
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
    NodeDeclarationList(NodeTypeQualifier* typeQualifier, const std::vector<Node*>& declarationList) :
        TypeQualifier(typeQualifier), DeclarationList(declarationList) { };

    ~NodeDeclarationList() override {
        delete TypeQualifier;
        for (auto& decl : DeclarationList) {
            delete decl;
        }
    };
};

class NodeMemberCall : public Node
{
    Node* Identifier = nullptr;
    Node* CallExpr = nullptr;
    bool IsArrow = false;

public:
    std::string print() override {
        std::string fprint = Identifier->print() + 
            (IsArrow ? "->" : + ".") + CallExpr->print();
        return fprint;
    };

    NodeMemberCall(NodeIdentifier* identifier, Node* callExpr, bool isArrow) :
        Identifier(identifier), CallExpr(callExpr), IsArrow(isArrow) {
    };

    ~NodeMemberCall() override {
        delete Identifier; Identifier = nullptr;
        delete CallExpr; CallExpr = nullptr;
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

class NodeUsing : public Node
{
public:
    NodeUsing() {};
    std::string print() override { return ""; };
    ~NodeUsing() {};
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

class NodeNew : public Node
{
    Node* Call = nullptr;
public:
    NodeNew(Node* call) : Call(call) {
    };

    std::string print() override {
        std::string fprint = "new " + (Call ? Call->print() : "");
        return fprint;
    };

    ~NodeNew() {
        delete Call;
    };
};

class NodeDelete : public Node
{
public:
    NodeDelete() {};

    std::string print() override {
        std::string fprint = "delete";
        return fprint;
    };

    ~NodeDelete() {};
};

class NodeNullptr : public Node
{
public:
    NodeNullptr() {};

    std::string print() override {
        std::string fprint = "nullptr";
        return fprint;
    };

    ~NodeNullptr() {};
};

class NodeCall : public Node
{
    std::string Name = "";
    std::vector<Node*> ArgumentList;
public:
    NodeCall(
        std::string name, const std::vector<Node*> argumentList, bool isHeapAllocated = false) :
        Name(name), ArgumentList(argumentList) {
    };

    std::string print() override {
        std::string fprint = Name;
        fprint += "(";
        int size = ArgumentList.size();
        for (size_t i = 0; i < size; i++)
            if (auto Decl = ArgumentList[i]; Decl)
                fprint += Decl->print() + (i == size - 1 ? "" : ", ");

        fprint += ")";
        return fprint;
    };

    ~NodeCall() {
        for (auto& i : ArgumentList) delete i;
    };
};

class NodeConstructor : public Node {
private:
    std::vector<Node*> ArgumentList;
public:
    NodeConstructor(const std::vector<Node*> argumentList) : 
        ArgumentList(argumentList) { };

    std::string print() override {
        std::string fprint = "contructor ";
        fprint += "(";
        int size = ArgumentList.size();
        for (size_t i = 0; i < size; i++)
            if (auto Decl = ArgumentList[i]; Decl)
                fprint += Decl->print() + (i == size - 1 ? "" : ", ");

        fprint += ")";
        return fprint;
    }

    ~NodeConstructor() override { }
};

class NodeDestructor : public Node {

public:
    NodeDestructor() { };

    std::string print() override {
        std::string fprint = "destructor()";
        return fprint;
    }

    ~NodeDestructor() override {}
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

// Generic parameter: T, K = int
class NodeGenericParam : public Node {
    std::string Name;
    bool Contruct = false;
    Node* Default; // может быть nullptr
public:
    NodeGenericParam(const std::string& name, bool construct, Node* def = nullptr)
        : Name(name), Contruct(construct), Default(def) {
    }

    std::string print() override {
        std::string res = Name;
        if (Default) res += " = " + Default->print() + (Contruct ? "()" : "");
        return res;
    }

    ~NodeGenericParam() override {
        delete Default;
    }
};

// Generic parameter list: [T, K = int, W]
class NodeGenericParams : public Node {
    std::vector<NodeGenericParam*> Params;
public:
    void add(NodeGenericParam* p) { if (p) Params.push_back(p); }

    std::string print() override {
        std::string res = "[";
        for (size_t i = 0; i < Params.size(); ++i) {
            res += Params[i]->print();
            if (i + 1 < Params.size()) res += ", ";
        }
        res += "]";
        return res;
    }

    const std::vector<NodeGenericParam*>& getParams() const { return Params; }

    ~NodeGenericParams() override {
        for (auto* p : Params) delete p;
    }
};

// Generic parameter list concretic: [int, 5, std::string]
class NodeGenericParamsConcretic : public Node {
    std::vector<Node*> Params;
public:
    void add(Node* p) { if (p) Params.push_back(p); }

    const std::vector<Node*>& getParams() const { return Params; }

    std::string print() override {
        std::string res = "[";
        for (size_t i = 0; i < Params.size(); ++i) {
            res += Params[i]->print();
            if (i + 1 < Params.size()) res += ", ";
        }
        res += "]";
        return res;
    }

    ~NodeGenericParamsConcretic() override {
        for (auto* p : Params) delete p;
    }
};

class NodeClass : public Node {
public:
    enum class INHERITANCE_TYPE { PUBLIC, PRIVATE };

private:
    std::string Name;
    Node* GenericParams = nullptr;
    Node* GenericConcretic = nullptr;
    std::string BaseClass;
    INHERITANCE_TYPE Type = INHERITANCE_TYPE::PRIVATE;
    Node* Body = nullptr;
public:
    NodeClass(
        const std::string& name,
        Node* generics = nullptr,
        Node* genericsConcretic = nullptr,
        const std::string& baseClass = "",
        INHERITANCE_TYPE type = INHERITANCE_TYPE::PRIVATE,
        Node* body = nullptr
    )
        : Name(name), GenericParams(generics), GenericConcretic(genericsConcretic), BaseClass(baseClass), Type(type), Body(body) {
    }

    std::string print() override {
        std::string fprint = "class " + Name;
        if (GenericParams) fprint += GenericParams->print();

        if (!BaseClass.empty()) {
            std::string type = (Type == INHERITANCE_TYPE::PRIVATE ? "" : "public ");
            fprint += " : " + type + BaseClass;
            if (GenericConcretic) fprint += GenericConcretic->print();
        }
        if (Body) fprint += " " + Body->print();

        return fprint;
    }

    ~NodeClass() override {
        delete GenericConcretic;
        delete GenericParams;
        delete Body;
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

struct NodeProperty : Node {
    std::string Name, Getter, Setter;
    NodeTypeQualifier* TypeQualifier = nullptr;

    NodeProperty(
        const std::string& name,
        NodeTypeQualifier* typeQualifier,
        const std::string& getter,
        const std::string& setter) :
        Name(name), TypeQualifier(typeQualifier), Getter(getter), Setter(setter) {}

    std::string print() override {
        return "__property" + 
            TypeQualifier->print() + " " + Name + " " + "{\n" +
            (Getter.empty() ? "" : "write = " + Getter + ",") +
            (Setter.empty() ? "" : "read = " + Setter) +
            "\n}";
    }
    ~NodeProperty() {
        delete TypeQualifier;
    }
};

#endif // NODE_HPP