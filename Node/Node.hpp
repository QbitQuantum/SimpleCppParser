
#ifndef NODE_HPP
#define NODE_HPP
#pragma once

#include <string>
#include <memory>
#include <vector>

class Node
{
protected:
    int Type = -1;
    Node() {};
public:
    virtual std::string print() = 0;
    virtual ~Node() = default;
};

class NodeType : public Node
{
public:
    enum class EType 
    {
        NONE, POINTER, REF, RVALUE
    };
    Node* Type = nullptr;
    std::vector<Node*> TemplateArgs;
    bool IsConst = false;
    bool IsTemplate = false;
    EType eType = EType::NONE;
    std::string getSymbol() const {
        switch (eType)
        {
        case NodeType::EType::POINTER: return "*";
        case NodeType::EType::REF: return "&";
        case NodeType::EType::RVALUE: return "&&";
        default: return "";
        }
    }
public:
    NodeType(Node* type, bool isTemplate, bool isConst, EType etype, const std::vector<Node*>& templateArgv = {}) :
        Type(type), IsTemplate(isTemplate), IsConst(isConst), eType(etype), TemplateArgs(templateArgv) {};

    std::string print() override {
        
        std::string TemplateArg;
        if (!TemplateArgs.empty())
        {
            TemplateArg += "<";
            int size = TemplateArgs.size();
            for (size_t i = 0; i < size; i++)
                TemplateArg += TemplateArgs[i]->print() + (i == size - 1 ? "" : ", ");
            TemplateArg += ">";
        }
        const std::string& OpenBracket = IsTemplate ? "" : "[";
        const std::string& CloseBracket = IsTemplate ? "" : "]";
        std::string fprint = OpenBracket + (IsConst ? std::string("const ") : std::string("")) + Type->print() + TemplateArg +
            getSymbol() + CloseBracket;
        return fprint;
    };

    ~NodeType() {
        for (auto& i : TemplateArgs) delete i;
        delete Type;
    }
};

class NodeIdentifier : public Node
{
    std::string Name = "";
    Node* Scope = nullptr;
public:
    NodeIdentifier(const std::string& name, Node* scope) :
        Name(name), Scope(scope) {
    };
    std::string print() override { 
        return (Scope ? Scope->print() : "") + Name;
    };
    ~NodeIdentifier() override {
        delete Scope;
    };
};

class NodeDeclaration : public Node
{
    Node* Identifier = nullptr;
    Node* Initializer = nullptr;
public:
    std::string print() override {
        std::string fprint = "";
        if (Identifier)
            fprint += Identifier->print();
        if (Initializer)
            fprint += " = " + Initializer->print();
        return fprint;
    };
    NodeDeclaration(Node* identifier, Node* initializer) :
        Identifier(identifier), Initializer(initializer) {
    };

    ~NodeDeclaration() override {
        delete Identifier; Identifier = nullptr;
        delete Initializer; Initializer = nullptr;
    };
};

class NodeDeclarationList : public Node
{
    Node* Type = nullptr;
    std::vector<Node*> DeclarationList;
public:
    std::string print() override {
        std::string fprint = "var";
        if (Type)
        {
            int size = DeclarationList.size();
            fprint += Type->print();
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
    NodeDeclarationList(Node* type, const std::vector<Node*>& declarationList) :
        Type(type), DeclarationList(declarationList) { };

    ~NodeDeclarationList() override {
        delete Type;
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

    NodeMemberCall(Node* identifier, Node* callExpr, bool isArrow) :
        Identifier(identifier), CallExpr(callExpr), IsArrow(isArrow) {
    };

    ~NodeMemberCall() override {
        delete Identifier; Identifier = nullptr;
        delete CallExpr; CallExpr = nullptr;
    };
};

class NodeScope : public Node
{
    std::vector<std::string> Scope;
public:
    NodeScope(const std::vector<std::string>& scope) :
        Scope(scope) {
    };
    std::string print() override { 
        std::string fprint = "";
        int size = Scope.size();
        for (size_t i = 0; i < size; i++)
            fprint += Scope[i] + "::";
        return fprint;
    
    };
    ~NodeScope() {
    };
};

class NodeAccess : public Node
{
    std::string Name;
    Node* Scope = nullptr;
public:
    NodeAccess(const std::string& name, Node* scope) :
        Name(name), Scope(scope) {};
    std::string print() override { return "access " + Name + " = " + Scope->print(); };
    ~NodeAccess() {
        delete Scope;
    };
};

class NodeUsing : public Node
{
public:
    std::string Name;
    Node* Path = nullptr;
public:
    NodeUsing(const std::string& name, Node* path) :
        Name(name), Path(path) {
    };
    std::string print() override { return "using " + Name + " = " + Path->print() + ";"; };
    ~NodeUsing() {
        delete Path;
    };
};

class NodeFunction : public Node
{
    Node* Type = nullptr;
    Node* Name = nullptr;
    std::vector<Node*> ArgumentList;
    Node* Body = nullptr;
public:
    NodeFunction(
        Node* type, Node* name, const std::vector<Node*> argumentList, Node* body = nullptr) :
        Type(type), Name(name),  ArgumentList(argumentList), Body(body) { };

    std::string print() override {  
        std::string fprint = "function "  + Type->print() + " " + Name->print();
        
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
        delete Type; Type = nullptr;
        delete Name; Name = nullptr;
        for (auto& i : ArgumentList) delete i;
        delete Body; Body = nullptr;
    };
};

class NodeLambda : public Node
{
    Node* Type = nullptr;
    Node* Name = nullptr;
    std::vector<Node*> ArgumentList;
    Node* Body = nullptr;
public:
    NodeLambda(
        Node* type, Node* name, const std::vector<Node*> argumentList, Node* body = nullptr) :
        Type(type), Name(name), ArgumentList(argumentList), Body(body) {
    };

    std::string print() override {
        std::string fprint = "lambda " + Type->print() + " " + Name->print();

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

    ~NodeLambda() {
        delete Type; Type = nullptr;
        delete Name; Name = nullptr;
        for (auto& i : ArgumentList) delete i;
        delete Body; Body = nullptr;
    };
};

class NodeConstructor : public Node {
private:
    std::vector<Node*> ArgumentList;
    Node* Body = nullptr;
public:
    NodeConstructor(const std::vector<Node*> argumentList, Node* body) :
        ArgumentList(argumentList), Body(body) {
    };

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

    ~NodeConstructor() override {
        for (auto& Argument : ArgumentList)
            delete Argument;
        delete Body;
    }
};

class NodeDestructor : public Node {
    Node* Body = nullptr;
public:
    NodeDestructor(Node* body) : Body(body) {};

    std::string print() override {
        std::string fprint = "destructor()";
        return fprint;
    }

    ~NodeDestructor() override {
        delete Body;
    }
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

class NodeCall : public Node
{
    Node* Name = nullptr;
    std::vector<Node*> ArgumentConcreticList;
public:
    NodeCall(
        Node* name, const std::vector<Node*> argumentConcreticList) :
        Name(name), ArgumentConcreticList(argumentConcreticList) {
    };

    std::string print() override {
        if (!Name) return "";
        std::string fprint = Name->print();
        fprint += "(";
        int size = ArgumentConcreticList.size();
        for (size_t i = 0; i < size; i++)
            if (auto Decl = ArgumentConcreticList[i]; Decl)
                fprint += Decl->print() + (i == size - 1 ? "" : ", ");

        fprint += ")";
        return fprint;
    };

    ~NodeCall() {
        delete Name;
        for (auto& i : ArgumentConcreticList) delete i;
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
        std::string fprint = "{\n ";
        int size = Statements.size();
        for (int i = 0; i < size; ++i) {
            if (auto stmt = Statements[i]) {
                fprint += stmt->print() + ";\n";
            }
        }
        fprint += " \n}";
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
    std::vector<Node*> Params;
public:
    NodeGenericParams(std::vector<Node*> params) :
        Params(params) { };

    std::string print() override {
        std::string res = "[";
        for (size_t i = 0; i < Params.size(); ++i) {
            res += Params[i]->print();
            if (i + 1 < Params.size()) res += ", ";
        }
        res += "]";
        return res;
    }

    const std::vector<Node*>& getParams() const { return Params; }

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
    enum class INHERITANCE_TYPE { PUBLIC, PRIVATE, STATIC };

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

class NodeStruct : public Node {
public:
    enum class INHERITANCE_TYPE { PUBLIC, STATIC };

private:
    std::string Name;
    Node* GenericParams = nullptr;
    INHERITANCE_TYPE Type = INHERITANCE_TYPE::PUBLIC;
    Node* Body = nullptr;
public:
    NodeStruct(
        const std::string& name,
        Node* generics = nullptr,
        INHERITANCE_TYPE type = INHERITANCE_TYPE::PUBLIC,
        Node* body = nullptr
    )
        : Name(name), GenericParams(generics), Type(type), Body(body) {
    }

    std::string print() override {
        std::string fprint = "struct " + Name;
        if (GenericParams) fprint += GenericParams->print();
        if (Body) fprint += " " + Body->print();
        return fprint;
    }

    ~NodeStruct() override {
        delete GenericParams;
        delete Body;
    }
};

class NodeNamespace : public Node {
private:
    Node* Name = nullptr;
    Node* Body = nullptr;
public:
    NodeNamespace(Node* name, Node* body)
        : Name(name), Body(body) {
    }

    std::string print() override {
        std::string fprint = "namespace " + Name->print();
        if (Body) fprint += " " + Body->print();
        return fprint;
    }

    ~NodeNamespace() override {
        delete Body;
        delete Name;
    }
};

class NodeProperty : public Node {
    std::string Name;
    Node* Type = nullptr;
    Node* Getter = nullptr, * Setter = nullptr;
public:
    NodeProperty(
        const std::string& name,
        Node* type,
        Node* getter,
        Node* setter) :
        Name(name), Type(type), Getter(getter), Setter(setter) {}

    std::string print() override {
        return "__property" + 
            Type->print() + " " + Name + " " + "{\n" +
            (!Getter ? "" : "write = " + Getter->print() + ",") +
            (!Setter ? "" : "read = " + Setter->print()) +
            "\n}";
    }
    ~NodeProperty() {
        delete Type;
    }
};

class NodeInteger : public Node {
private:
    std::string raw_value;
public:
    NodeInteger(const std::string& val) : raw_value(val) {}

    std::string print() override {
        return raw_value;
    }
};

class NodeFloating : public Node {
private:
    std::string raw_value;
public:
    NodeFloating(const std::string& val) : raw_value(val) {}

    std::string print() override {
        return raw_value;
    }
};

class NodeString : public Node {
private:
    std::string raw_value;
public:
    NodeString(const std::string& val) : raw_value(val) {}

    std::string print() override {
        return "\"" + raw_value + "\"";
    }
};

class NodeCharacter : public Node {
private:
    std::string raw_value;
public:
    NodeCharacter(const std::string& val) : raw_value(val) {}

    std::string print() override {
        return "'" + std::string(1, raw_value[0]) + "'";
    }
};

class NodeBoolean : public Node {
private:
    std::string raw_value;
public:
    NodeBoolean(const std::string& val) : raw_value(val) {}

    std::string print() override {
        return raw_value;
    }
};

class NodeNullptr : public Node {
public:
    std::string print() override {
        return "nullptr";
    }
};

class NodeDefault : public Node {
public:
    std::string print() override {
        return "default";
    }
};

class NodeWhile : public Node {
    Node* Condition = nullptr;
    Node* Body = nullptr;
    bool IsDoWhile = false;
public:
    NodeWhile(Node* condition, Node* body, bool isDoWhile) :
        Condition(condition), Body(body), IsDoWhile(isDoWhile) {}
    std::string print() override {
        std::string fprint = "while ";
        fprint += (IsDoWhile ? "do " : "");
        fprint += "(" + (Condition ? Condition->print() : "") + ")";
        fprint += (Body ? Body->print() : "");
        return fprint;
    }
    ~NodeWhile() {
        delete Body;
        delete Condition;
    }
};

class NodeTryCatch : public Node {
    Node* BodyTry = nullptr;
    Node* BodyCatch = nullptr;
    Node* Declaration = nullptr;
public:
    NodeTryCatch(Node* bodytry, Node* bodycatch, Node* declaration) :
        BodyTry(bodytry), BodyCatch(bodycatch), Declaration(declaration) {
    }
    std::string print() override {
        if (!BodyTry) return "BodyTry empty";
        std::string fprint = "try " + BodyTry->print();
        if (BodyCatch)
        {
            fprint += "\ncatch " + (Declaration ? "(" + Declaration->print() + ")" : "") + BodyCatch->print();
        }
        return fprint;
    }
    ~NodeTryCatch() {
        delete Declaration;
        delete BodyCatch;
        delete BodyTry;
    }
};

class NodeBinaryOp : public Node {
public:
    enum class BinaryOp
    {
        Unknown, Plus, Minus, Asterisk, Slash
    };
private:
    BinaryOp Op = BinaryOp::Unknown;
    Node* Left = nullptr;
    Node* Right = nullptr;
    std::string getSymbol()
    {
        switch (Op) {
        case BinaryOp::Plus: return "+";
        case BinaryOp::Minus: return "-";
        case BinaryOp::Asterisk: return "*";
        case BinaryOp::Slash: return "/";
        default: return "";
        }
    }
public:
    NodeBinaryOp(const BinaryOp& op, Node* left, Node* right)
        : Op(op), Left(left), Right(right) {
    }

    std::string print() override {
        if (Op == BinaryOp::Unknown)
            return "";
        if (!Left || !Right)
            return "";
        return Left->print() + " " + getSymbol() + " " + Right->print();
    }
    ~NodeBinaryOp() {
        delete Left;
        delete Right;
    }
};

class NodeUnaryOp : public Node {
public:
    enum class UnaryOp
    {
        Unknown, Minus
    };
private:
    UnaryOp Op = UnaryOp::Unknown;
    Node* Right = nullptr;
    std::string getSymbol()
    {
        switch (Op) {
        case UnaryOp::Minus: return "-";
        default: return "";
        }
    }
public:
    NodeUnaryOp(const UnaryOp& op, Node* right)
        : Op(op), Right(right) {
    }

    std::string print() override {
        if (!Right)
            return "";
        return getSymbol() + Right->print();
    }
    ~NodeUnaryOp() {
        delete Right;
    }
};

#endif // NODE_HPP