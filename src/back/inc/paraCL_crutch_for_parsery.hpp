#ifndef PARACL_CRUTCH_FOR_PARSERY
#define PARACL_CRUTCH_FOR_PARSERY

#include <cstddef>
#include <unordered_map>
#include <string>
#include <vector>
#include <string>
#include <memory>

namespace ParaCL {

enum class token_t
{
    // and, or, not tokens
    AND,
    OR,
    NOT,

    // +, -, *,  /  tokens
    ADD,
    SUB,
    MUL,
    DIV,

    UNMINUS, /* - */
    UNPLUS,  /* + */

    // combined assignment operators
    ADDASGN, /* += */
    SUBASGN, /* -= */
    MULASGN, /* *= */
    DIVASGN, /* /= */
    INC,     /* ++ */
    DEC,     /* -- */

    // >, >=, <, <=, ==, !=  tokens
    ISAB,
    ISABE,
    ISLS,
    ISLSE,
    ISEQ,
    ISNE,

    // (, ), {, }  tokens
    LCIB,
    RCIB,
    LCUB,
    RCUB,

    // while, input, =, print, if, else, else if tokens
    WH,
    IN,
    AS,
    PRINT,

    IF,
    ELSE,
    ELIF,

    // number, variable, semicolon  tokens 
    NUM,
    VAR,
    SC,

    // end of translation token
    EOT
};

const std::unordered_map<std::string, token_t> tokenMap =
{
    { "and",   token_t::AND   },
    { "or",    token_t::OR    },
    { "not",   token_t::NOT   },

    { "+",     token_t::ADD   },
    { "-",     token_t::SUB   },
    { "*",     token_t::MUL   },
    { "/",     token_t::DIV   },

    { "+=",     token_t::ADDASGN},
    { "-=",     token_t::SUBASGN},
    { "*=",     token_t::MULASGN},
    { "/=",     token_t::DIVASGN},

    { ">",     token_t::ISAB  },
    { ">=",    token_t::ISABE },
    { "<",     token_t::ISLS  },
    { "<=",    token_t::ISLSE },
    { "==",    token_t::ISEQ  },
    { "!=",    token_t::ISNE  },

    { "(",     token_t::LCIB  },
    { ")",     token_t::RCIB  },
    { "{",     token_t::LCUB  },
    { "}",     token_t::RCUB  },

    { "while", token_t::WH    },
    { "?",     token_t::IN    },
    { "=",     token_t::AS    },
    { "print", token_t::PRINT },
    { "if",    token_t::IF    },
    { "elif",  token_t::ELIF  },
    { "else",  token_t::ELSE  },

    { ";",     token_t::SC    }
};

const std::unordered_map<token_t, std::string> reverseTokenMap = []
{
    std::unordered_map<token_t, std::string> rev;

    for (const auto& [key, val] : tokenMap) rev[val] = key;
    
    return rev;
} ();



struct ASTNode {
    virtual ~ASTNode() = default;
};

// expressions
struct Expr : ASTNode {};

struct NumExpr : Expr {
    int value;
    NumExpr(int v) : value(v) {}
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct InputExpr : Expr {};

struct UnExpr : Expr {
    token_t op;
    std::unique_ptr<Expr> operand;
    UnExpr(token_t op, std::unique_ptr<Expr> v)
        : op(op), operand(std::move(v)) {}
};

struct BinExpr : Expr {
    token_t op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    BinExpr(token_t op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}
};

struct AssignExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignExpr(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct CombinedAssingExpr : Expr
{
    token_t op;
    std::string name;
    std::unique_ptr<Expr> value;
    CombinedAssingExpr(token_t op, std::string n, std::unique_ptr<Expr> value)
        : op(op), name(std::move(n)), value(std::move(value)) {} 
};

// statements
struct Stmt : ASTNode {};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct CombinedAssingStmt : Stmt
{
    token_t op;
    std::string name;
    std::unique_ptr<Expr> value;
    CombinedAssingStmt(token_t op, std::string n, std::unique_ptr<Expr> value)
        : op(op), name(std::move(n)), value(std::move(value)) {}
};

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expr;
    PrintStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

struct BlockStmt : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> nodes;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<ASTNode>> body)
        : nodes(std::move(body)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct IfCondition : ASTNode {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    IfCondition(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ElifCondition : ASTNode {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    ElifCondition(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ElseCondition : ASTNode {
    std::unique_ptr<BlockStmt> body;
    ElseCondition(std::unique_ptr<BlockStmt> b)
        :  body(std::move(b)) {}
};

struct Condition : ASTNode {
    std::unique_ptr<IfCondition>   if_stmt;
    std::unique_ptr<ElseCondition> else_stmt;
    // std::vector<std::unique_ptr<ElifCondition>> elif_stmts;

    Condition(std::unique_ptr<IfCondition> base_if_stmt)
        : if_stmt(std::move(base_if_stmt)), else_stmt(nullptr) {}

    void add_else_condition(std::unique_ptr<ElseCondition> else_cond) {
        else_stmt = std::move(else_cond);
    }

};

struct ProgramAST {
    std::vector<std::unique_ptr<ASTNode>> nodes;
};

void
compile(const ParaCL::ProgramAST& progAST);
    
void
dump(const ProgramAST& progAST, const std::string& filename = "dot-out/ast.dot");

}; // namespace ParaCL

#endif // PARACL_CRUTCH_FOR_PARSERY