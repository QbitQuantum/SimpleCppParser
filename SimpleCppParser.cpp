
#include "Lexer.hpp"
#include "PostLexer.hpp"
#include "PreParser.hpp"
#include "ParserEngine.hpp"

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
    std::string code = ReadFile("code.mylang");
    
    Lexer lexer(code);
    auto lexerbuffer = lexer.GetBufferLexerToken();
    
    PostLexer postLexer(lexerbuffer);
    auto postlexerbuffer = postLexer.GetBufferPostLexerToken();

    PreParser preParser(postlexerbuffer);
    auto preParserbuffer = preParser.GetBufferPreParserToken();

    if (false)
    {
        for (auto& Tok : postlexerbuffer)
            if (Tok.type != TTokenID::Space && Tok.type != TTokenID::LineFeed)
                std::cout << NameTTokenID(Tok.type) << " |" << Tok.value << "|\n";
    }
}