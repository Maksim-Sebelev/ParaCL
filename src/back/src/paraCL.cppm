module;
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "paraCL.hpp" 

export module paraCL;

void dumpExpr(std::ostream& out, const ParaCL::Parser::Expr* expr);
void dumpStmt(std::ostream& out, const ParaCL::Parser::Stmt* stmt);

std::string ptrToStr(const void* ptr) {
    std::ostringstream oss;
    oss << ptr;
    return oss.str();
}

export namespace ParaCL::Parser {


void dump(ProgramAST& progAST, const std::string& filename = "imgs/ast.dot")
{
    std::ofstream out(filename);
    out << "digraph AST {\n";
    out << "  node [shape=box];\n";

    std::string rootId = "Program";
    out << "  \"" << rootId << "\" [label=\"Program\"];\n";

    for (auto& stmt : progAST.statements)
    {
        dumpStmt(out, stmt.get());
        out << "  \"" << rootId << "\" -> \"" << ptrToStr(stmt.get()) << "\";\n";
    }

    out << "}\n";
    out.close();

    std::string dot_cmd = "dot -Tsvg " + filename + " -o imgs/ast.svg";
}

}

using namespace ParaCL::Parser;

void dumpExpr(std::ostream& out, const Expr* expr) {
    std::string nodeId = ptrToStr(expr);
    std::string label;

    if (auto bin = dynamic_cast<const BinExpr*>(expr)) {
        label = ParaCL::reverseTokenMap.at(bin->op);
        out << "  \"" << nodeId << "\" [label=\"" << label << "\", style=filled, fillcolor=\"lightyellow\"];\n";

        
        auto leftExpr =  dynamic_cast<const Expr*>(&bin->left);
        auto rightExpr = dynamic_cast<const Expr*>(&bin->right);

        if (!leftExpr || !rightExpr) {
            throw std::runtime_error("BinExpr children are not Expr");
        }

        dumpExpr(out, leftExpr);
        dumpExpr(out, rightExpr);

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(&bin->left) << "\";\n";
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(&bin->right) << "\";\n";
    }
    else if (auto num = dynamic_cast<const NumExpr*>(expr)) {
        label = "Num: " + std::to_string(num->value);
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else if (auto var = dynamic_cast<const VarExpr*>(expr)) {
        label = "Var: " + var->name;
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else if (auto in = dynamic_cast<const InputExpr*>(expr)) {
        label = "Input";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
}; // namespace ParaCL::Parser


void dumpStmt(std::ostream& out, const Stmt* stmt) {
    std::string nodeId = ptrToStr(stmt);
    std::string label;

    if (auto assign = dynamic_cast<const AssignStmt*>(stmt))
    {
        label = "Assign: " + assign->name;
        out << "  \"" << nodeId << "\" [label=\"" << label << "\", style=filled, fillcolor=\"lightblue\"];\n";

        
        auto expr = dynamic_cast<const Expr*>(&assign->value);
        if (!expr) throw std::runtime_error("AssignStmt value is not Expr");

        dumpExpr(out, expr);

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(&assign->value) << "\";\n";
    }
    else if (auto print = dynamic_cast<const PrintStmt*>(stmt))
    {
        label = "Print";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        
        auto expr = dynamic_cast<const Expr*>(&print->expr);
        if (!expr) throw std::runtime_error("PrintStmt expr is not Expr");

        dumpExpr(out, expr);

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(&print->expr) << "\";\n";
    }
    else if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt))
    {
        label = "While";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        dumpExpr(out, whileStmt->condition.get()); 
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(whileStmt->condition.get())
            << "\" [label=\"cond\", fontcolor=\"gray50\"];\n";

        for (auto& s : whileStmt->body->statements)
        {
            dumpStmt(out, s.get());
            out << " " << nodeId << "\" -> \"" << ptrToStr(s.get())
                << "\" [label=\"body\", fontcolor=\"gray50\"];\n";
        }
    }
    else if (auto block = dynamic_cast<const BlockStmt*>(stmt))
    {
        label = "Block";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        for (auto& s : block->statements)
        {
            dumpStmt(out, s.get()); 
            out << "  \"" << nodeId << "\" -> \"" << ptrToStr(s.get()) << "\";\n";
        }
    }
}
