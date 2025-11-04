#include "token_t.hpp"

#include <string>
#include <list>
#include <vector>
#include <variant>
#include <stdexcept>
#include <iostream>

namespace ParaCL::Lexer {

using tokenData_t = std::pair<token_t, std::variant<int, std::string>>;

static tokenData_t
createToken(const std::string& tokenWord);

std::vector<tokenData_t>
tokenize(const std::string& buffer)
{
    std::vector<tokenData_t> tokens;
    std::string tokenWord;
    
    auto createToken_lambdaWrap = [&](std::string& tokenWord) {
        if (!tokenWord.empty())
        {
            tokens.push_back(createToken(tokenWord));
            tokenWord.clear();  
        }
    };

    for (std::string::const_iterator i = buffer.begin(), end = buffer.end(); i != end; ++i)
    {

        if (*i == '\n') {}
        else if (*i == ' ' || *i == ';')
        {
            createToken_lambdaWrap(tokenWord);

            if (*i == ';')
            {
                tokens.push_back(createToken(";"));
            }
        }
        else if (*i == '(' || *i == ')' || *i == '{' || *i == '}')
        {
            createToken_lambdaWrap(tokenWord);
            tokens.push_back(createToken(std::string(1, *i)));
        }
        else
        {
            tokenWord += *i;
        }
    }
    tokens.emplace_back(token_t::EOT, std::string{});
    return tokens;
}

static tokenData_t
createToken(const std::string& tokenWord)
{
    auto tokenType = tokenMap.find(tokenWord);

    tokenData_t tokenData;

    if (tokenType == tokenMap.end()) // it's variable or number
    {
        if (std::isalpha(tokenWord[0])) // variable if first symbol is letter
        {
            tokenData.first = token_t::VAR;
            tokenData.second = tokenWord;
        }
        else if (std::isdigit(tokenWord[0]))
        {
            tokenData.first = token_t::NUM;
            tokenData.second = std::stoi(tokenWord);
        }
        else
        {
            if (tokenWord.empty()) throw std::runtime_error("Token is empty");

            throw std::runtime_error("Unknown token: \"" + tokenWord + "\"");
        }
    }
    else
    {
        tokenData.first = tokenType->second;
    }

    return tokenData;
}

void
dump(const std::vector<tokenData_t>& tokens)
{
    for (const auto& [type, value] : tokens)
    {
        auto tokenWordIter = reverseTokenMap.find(type);
        if (tokenWordIter != reverseTokenMap.end())
        {
            std::cout << tokenWordIter->second;
        }
        else
        {
            if (type == token_t::VAR)
            {
                std::cout << "Variable: " << std::get<std::string>(value);
            }
            else if (type == token_t::NUM)
            {
                std::cout << "Number: " << std::get<int>(value);
            }
            else if (type == token_t::EOT)
            {
                std::cout << "EOT";
            }
            else
            {
                throw std::runtime_error("Unknown token type: " + 
                                          std::to_string(static_cast<int>(type)));
            }
        }
        std::cout << " ";
    }
    std::cout << "\n";
}

}; // namespace ParaCL::Lexer