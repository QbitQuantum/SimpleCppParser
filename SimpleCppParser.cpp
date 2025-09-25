
#include "LexerEngineBasic.hpp"
#include "LexerEngineAdvance.hpp"

#include <iostream>
#include <string>

std::string code = R"(
class EUpdateError : EDatabaseError {
private:
    int FErrorCode;
    int FPreviousError;
    int FPosition;
    std::string FContext;
    Exception FOriginalException;
public:
    TIDDeclaration MatchBinarOperatorWithTuple(TSContext SContext, TIDExpression CArray, TIDExpression SecondArg);
    void ParseEnumType(TScope Scope, TIDEnum Decl);
    EUpdateError(const std::string NativeError, const std::string Context, int ErrCode, int PrevError, Exception E);
    EUpdateError() override;

    std::string Context() {
        return FContext;
    }
    int ErrorCode() {
        return FErrorCode;
    }
    int PreviousError() {
        return FPreviousError;
    }
    Exception OriginalException() {
        return FOriginalException;
    }
    int Position() {
        return FPosition;
    }
    void Position(int _Position) {
        FPosition = _Position;
    }
};

)";

int main()
{
    LexerEngineBasic lexEngineBasic(code);
    auto lexbuffer = lexEngineBasic.GetBufferToken();
    
    if (false)
    {
        for (auto& Tok : lexbuffer)
            if (Tok.type != TTokenID::Space && Tok.type != TTokenID::LineFeed)
                std::cout << NameTTokenID(Tok.type) << " |" << Tok.value << "|\n";
    }
}