#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <unordered_map>
#include <string>

namespace ParaCL {

enum class token_t
{
    // +, -, *, /  tokens
    ADD,
    SUB,
    MUL,
    DIV,

    // >, >=, <, <=, ==  tokens
    ISAB,
    ISABE,
    ISLS,
    ISLSE,
    ISEQ,

    // (, ), {, }  tokens
    LCIB,
    RCIB,
    LCUB,
    RCUB,

    // while, input, =, print  tokens
    WH,
    IN,
    AS,
    PRINT,

    // number, variable, semicolon  tokens 
    NUM,
    VAR,
    SC,

    // end of translation  token
    EOT
};

const std::unordered_map<std::string, token_t> tokenMap =
{
    { "+",     token_t::ADD   },
    { "-",     token_t::SUB   },
    { "*",     token_t::MUL   },
    { "/",     token_t::DIV   },

    { ">",     token_t::ISAB  },
    { ">=",    token_t::ISABE },
    { "<",     token_t::ISLS  },
    { "<=",    token_t::ISLSE },
    { "==",    token_t::ISEQ  },

    { "(",     token_t::LCIB  },
    { ")",     token_t::RCIB  },
    { "{",     token_t::LCUB  },
    { "}",     token_t::RCUB  },

    { "while", token_t::WH    },
    { "?",     token_t::IN    },
    { "=",     token_t::AS    },
    { "print", token_t::PRINT },

    { ";",     token_t::SC    }
};

const std::unordered_map<token_t, std::string> reverseTokenMap = []
{
    std::unordered_map<token_t, std::string> rev;

    for (const auto& [key, val] : tokenMap) rev[val] = key;
    
    return rev;
} ();

}; // namespace ParaCL

#endif // TOKENS_HPP