#include "paraCL.hpp"

#include <iostream>
#include <vector>

int main() try
{
    std::string buffer;
    std::getline(std::cin, buffer, '\0');

    std::vector<ParaCL::Lexer::tokenData_t> tokens = ParaCL::Lexer::tokenize(buffer);
    ParaCL::Lexer::dump(tokens);

    ParaCL::Parser::ProgramAST progAST = ParaCL::Parser::createAST(tokens);

    ParaCL::Parser::dump(progAST);

}
catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
