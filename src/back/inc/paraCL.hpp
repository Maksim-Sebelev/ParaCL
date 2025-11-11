#ifndef PARACL_H
#define PARACL_H

#include "token_t.hpp"

#include <vector>
#include <variant>
#include <string>
#include <memory>

namespace ParaCL
{
// ================================ parser ================================

namespace Parser {

// Abstract Syntax Tree (AST) node
struct ASTNode {
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// expressions
struct Expr : ASTNode {};

struct NumExpr : Expr {
    int value;
    NumExpr(int v) : value(v) {}
};

struct VarExpr final : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct InputExpr final : Expr {};

struct BinExpr final : Expr {
    token_t op;
    Expr left;
    Expr right;
    BinExpr(token_t op, Expr lhs, Expr rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}
};

struct AssignExpr final : Expr {
    std::string name;
    Expr value;
    AssignExpr(std::string n, Expr v)
        : name(std::move(n)), value(std::move(v)) {}
};

// statements (=, print, block, while)
struct Stmt : ASTNode {};

struct AssignStmt final : Stmt {
    std::string name;
    Expr value;
    AssignStmt(std::string n, Expr v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct PrintStmt final : Stmt {
    Expr expr;
    PrintStmt(Expr e) : expr(std::move(e)) {}
};

struct BlockStmt final : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
};

struct WhileStmt final : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ProgramAST final : ASTNode {
    std::vector<std::unique_ptr<Stmt>> statements;
};

}; // namespace Parser

// ================================ compiler ================================
namespace Compiler {

void compileByCpp(const ParaCL::Parser::ProgramAST& progAST);




}; // namespace Compiler

}; // namespace ParaCL


#endif
