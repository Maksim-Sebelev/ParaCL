module;

#include <memory>
#include <vector>

export module ast;

namespace ParaCL
{
namespace ast
{
export class AST;

export enum class binary_op_t
{
    AND,
    OR,
    NOT,
    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    ISAB,
    ISABE,
    ISLS,
    ISLSE,
    ISEQ,
    ISNE,
};

export enum class unary_op_t
{
    MINUS,
    PLUS,
    NOT,
};

export enum class combined_assign_t
{
    ADDASGN, /* += */
    SUBASGN, /* -= */
    MULASGN, /* *= */
    DIVASGN, /* /= */
    REMASGN, /* %= */
};

export class ASTVisitor
{
};

class basic_node;
class expression_node;
class number_node;
class variable_node;
class input_operator_node;
class unary_operation_node;
class assignment_node;
class commbined_assignment_node;
class statement_node;
class assignment_statement_node;
class combined_assignment_statement_node;
class string_constant_node;
class print_node;
class block_node;
class while_node;
class condition_node;

export using basic_node_ptr = std::unique_ptr<basic_node>;

class basic_node
{
  public:
    virtual ~basic_node() = default;
};

// expressions
class expression_node : basic_node
{
};

class number_node : expression_node
{
  public:
    int value;
    number_node(int v) : value(v)
    {
    }
};

class variable_node : expression_node
{
    std::string name;
    variable_node(std::string n) : name(std::move(n))
    {
    }
};

class input_operator_node : expression_node
{
  public:
};

class unary_operation_node : expression_node
{
  private:
    const unary_op_t op_;

  public:
    unary_op_t op() const
    {
        return op_;
    }

    std::unique_ptr<expression_node> operand;
    unary_operation_node(unary_op_t op, std::unique_ptr<expression_node> v) : op_(op), operand(std::move(v))
    {
    }
};

class binary_operation_node : expression_node
{
  private:
    const binary_op_t op_;

  public:
    binary_op_t op() const
    {
        return op_;
    }

    std::unique_ptr<expression_node> left;
    std::unique_ptr<expression_node> right;

    binary_operation_node(binary_op_t op, std::unique_ptr<expression_node> lhs, std::unique_ptr<expression_node> rhs)
        : op_(op), left(std::move(lhs)), right(std::move(rhs))
    {
    }
};

class assignment_node : expression_node
{
    std::string name;
    std::unique_ptr<expression_node> value;
    assignment_node(std::string n, std::unique_ptr<expression_node> v) : name(std::move(n)), value(std::move(v))
    {
    }
};

class commbined_assignment_node : expression_node
{
  private:
    const combined_assign_t op_;

  public:
    combined_assign_t op() const
    {
        return op_;
    }
    std::string name;
    std::unique_ptr<expression_node> value;
    commbined_assignment_node(combined_assign_t op, std::string n, std::unique_ptr<expression_node> value)
        : op_(op), name(std::move(n)), value(std::move(value))
    {
    }
};

// statements
class statement_node : basic_node
{
};

class assignment_statement_node : statement_node
{
    std::string name;
    std::unique_ptr<expression_node> value;
    assignment_statement_node(std::string n, std::unique_ptr<expression_node> v)
        : name(std::move(n)), value(std::move(v))
    {
    }
};

class combined_assignment_statement_node : statement_node
{
  private:
    const combined_assign_t op_;

  public:
    combined_assign_t op() const
    {
        return op_;
    }

    std::string name;
    std::unique_ptr<expression_node> value;
    combined_assignment_statement_node(combined_assign_t op, std::string n, std::unique_ptr<expression_node> value)
        : op_(op), name(std::move(n)), value(std::move(value))
    {
    }
};

class string_constant_node : expression_node
{
    std::string value;
    string_constant_node(std::string str) : value(std::move(str))
    {
    }
};

class print_node : statement_node
{
    std::vector<std::unique_ptr<expression_node>> args;
    print_node(std::vector<std::unique_ptr<expression_node>> arguments) : args(std::move(arguments))
    {
    }
};

class block_node : statement_node
{
    std::vector<std::unique_ptr<statement_node>> statements;
    block_node() = default;
    block_node(std::vector<std::unique_ptr<statement_node>> stmts) : statements(std::move(stmts))
    {
    }
};

class while_node : statement_node
{
    std::unique_ptr<expression_node> condition;
    std::unique_ptr<block_node> body;
    while_node(std::unique_ptr<expression_node> cond, std::unique_ptr<block_node> b)
        : condition(std::move(cond)), body(std::move(b))
    {
    }
};

class if_condition
{
  protected:
    std::unique_ptr<expression_node> condition;
    std::unique_ptr<block_node> body;

    if_condition(std::unique_ptr<expression_node> cond, std::unique_ptr<block_node> b)
        : condition(std::move(cond)), body(std::move(b))
    {
    }
};

class else_if_conditions
{
  protected:
    struct else_if_condition
    {
        std::unique_ptr<expression_node> condition;
        std::unique_ptr<block_node> body;
    };

    std::vector<else_if_condition> conditions_and_bodies;

    // else_if_condition(std::unique_ptr<expression_node> cond, std::unique_ptr<block_node> b)
    // : condition(std::move(cond)), body(std::move(b))
    // {
    // }
};

class else_condition
{
  protected:
    std::unique_ptr<block_node> body;
    else_condition(std::unique_ptr<block_node> b) : body(std::move(b))
    {
    }
};

class condition_node : public basic_node, private if_condition, private else_if_conditions, private else_condition
{
    // ConditionStatement(std::unique_ptr<IfStatement> base_if_stmt) : if_stmt(std::move(base_if_stmt))
    // {
    // }

    // void add_elif_condition(std::unique_ptr<ElifStatement> elif_stmt)
    // {
    //     elif_stmts.push_back(std::move(elif_stmt));
    // }

    // void add_else_condition(std::unique_ptr<ElseStatement> else_stmt_)
    // {
    //     if (not else_stmt_.get())
    //         return;

    //     else_stmt = std::move(else_stmt_);
    // }
};

export class AST
{
  protected:
    std::vector<std::unique_ptr<basic_node>> statements;

  public:
    friend class ASTVisitor;

    static
};

} /* namespace ast */
} /* namespace ParaCL */
