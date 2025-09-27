
#include "LexerEngineBasic.hpp"
#include "LexerEngineAdvance.hpp"

#include <iostream>
#include <string>

#include <fstream>
#include <filesystem>

std::string ReadFile(std::string filepath) {

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file)
        return "";
    auto size = file.tellg();
    std::string content(size, '\0');
    file.seekg(0);
    file.read(&content[0], size);
    return content;
};


int main()
{
    std::string code = ReadFile("../CppCode.cpp");
    LexerEngineBasic lexEngineBasic(code);
    auto lexbufferBasic = lexEngineBasic.GetBufferLexerBasicToken();
    
    if (false)
    {
        for (auto& Tok : lexbufferBasic)
            if (Tok.type != TTokenID::Space && Tok.type != TTokenID::LineFeed)
                std::cout << NameTTokenID(Tok.type) << " |" << Tok.value << "|\n";
    }
}