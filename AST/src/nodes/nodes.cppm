module;

#include <utility>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <initializer_list>
#include <iostream>

export module ast_nodes;

export import node_type_erasure;

//--------------------------------------------------------------------------------------------------------------------------------------
namespace last::node
{
//--------------------------------------------------------------------------------------------------------------------------------------

export
class CodeLocation final
{
private:
    using code_place_uint_t = unsigned int;
private:
    std::string file_;

    code_place_uint_t line_begin_   = 0;
    code_place_uint_t line_end_     = 0;
    code_place_uint_t column_begin_ = 0;
    code_place_uint_t column_end_   = 0;

    std::string       code_excerpt_ = "";

public: /* getters */
    std::string_view file() const noexcept
    { return file_; }

    code_place_uint_t column_begin() const noexcept
    { return column_begin_; }
    code_place_uint_t column_end() const noexcept
    { return column_end_; }
    code_place_uint_t line_begin() const noexcept
    { return line_begin_; }
    code_place_uint_t line_end() const noexcept
    { return line_end_; }

    std::string_view code_excerpt() const & noexcept
    { return code_excerpt_;  }

    std::string&& code_excerpt() && noexcept
    { return std::move(code_excerpt_); }

public: /* setters */
    void set_file(std::string_view file)
    { file_ = file; }

    void set_line_begin(code_place_uint_t line_begin) noexcept
    { line_begin_ = line_begin; }

    void set_line_end(code_place_uint_t line_end) noexcept
    { line_end_ = line_end; }

    void set_column_begin(code_place_uint_t column_begin) noexcept
    { column_begin_ = column_begin; }

    void set_column_end(code_place_uint_t column_end) noexcept
    { column_end_ = column_end; }

    void set_code_excerpt(std::string_view code_excerpt) 
    { code_excerpt_ = code_excerpt; }

    void set_position(code_place_uint_t line_begin, code_place_uint_t line_end,
                                code_place_uint_t column_begin, code_place_uint_t column_end) noexcept
    {
        line_begin_ = line_begin;
        line_end_ = line_end;
        column_begin_ = column_begin;
        column_end_ = column_end;
    }

    void set_all(std::string_view file, code_place_uint_t line_begin, code_place_uint_t line_end,
                           code_place_uint_t column_begin, code_place_uint_t column_end,
                           std::string_view code_excerpt)
    {
        file_ = file;
        line_begin_ = line_begin;
        line_end_ = line_end;
        column_begin_ = column_begin;
        column_end_ = column_end;
        code_excerpt_ = code_excerpt;
    }

public:
    template <typename... Args>
    CodeLocation(Args...)
    {
        static_assert(false, "using unsecialized CodeLocation tmeplate constructor");
    }

    template <>
    CodeLocation()
    {}

    template <>
    CodeLocation(std::string_view file, code_place_uint_t lb, code_place_uint_t le, code_place_uint_t cb, code_place_uint_t ce) :
        file_(file),
        line_begin_(lb), line_end_(le), column_begin_(cb), column_end_(ce),
        code_excerpt_()
    {}
    template <>
    CodeLocation(std::string_view file, code_place_uint_t lb, code_place_uint_t le, code_place_uint_t cb, code_place_uint_t ce, std::string_view code_excerpt) :
        file_(file),
        line_begin_(lb), line_end_(le), column_begin_(cb), column_end_(ce),
        code_excerpt_(code_excerpt)
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Scope final : private std::vector<BasicNode>
{
public:
    using std::vector<BasicNode>::emplace_back;
    using std::vector<BasicNode>::push_back;
    using std::vector<BasicNode>::begin;
    using std::vector<BasicNode>::end;
    using std::vector<BasicNode>::size;
    using std::vector<BasicNode>::operator[];
    using std::vector<BasicNode>::back;

public:
    Scope() = default;

    Scope(size_t size) : std::vector<BasicNode>(size)
    {}

    Scope(std::vector<BasicNode>&& nodes) : std::vector<BasicNode>(std::move(nodes))
    {}

    Scope(std::vector<BasicNode> const & nodes) : std::vector<BasicNode>(nodes)
    {}

    Scope(std::initializer_list<BasicNode> il) : std::vector<BasicNode>(std::move(il))
    {}

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Variable final
{
private:
    std::string name_;
public:
    Variable(std::string&& name) : name_(std::move(name)) /* variale must own his name */
    {}
public:
    std::string_view name() const & noexcept
    { return name_; }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Print final : private std::vector<BasicNode>
{
public:
    using std::vector<BasicNode>::emplace_back;
    using std::vector<BasicNode>::push_back;
    using std::vector<BasicNode>::begin;
    using std::vector<BasicNode>::end;
    using std::vector<BasicNode>::size;
    using std::vector<BasicNode>::operator[];
    using std::vector<BasicNode>::iterator;
    using std::vector<BasicNode>::const_iterator;

public:
    Print() = default;
    Print(std::vector<BasicNode>&& args) : std::vector<BasicNode>(std::move(args))
    {}
    Print(std::initializer_list<BasicNode> il) : std::vector<BasicNode>(std::move(il))
    {}

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Scan final
{
public:
    Scan() = default;

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class UnaryOperator final
{
public:
    enum UnaryOperatorT
    { MINUS, PLUS, NOT };
private:
    BasicNode arg_;
    UnaryOperatorT type_;
public:
    UnaryOperator(UnaryOperatorT type, BasicNode const &arg) :
    arg_(arg), type_(type)
    {}

    UnaryOperator(UnaryOperatorT type, BasicNode&& arg) :
    arg_(std::move(arg)), type_(type)
    {}

public:
    UnaryOperatorT type() const noexcept
    { return type_; }

    BasicNode const &arg() const & noexcept
    { return arg_; }

    BasicNode &arg() & noexcept
    { return arg_; }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class BinaryOperator final
{
public:
    enum BinaryOperatorT
    {
        AND,
        OR,
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
        ASGN,    /* =  */
        ADDASGN, /* += */
        SUBASGN, /* -= */
        MULASGN, /* *= */
        DIVASGN, /* /= */
        REMASGN, /* %= */
    };

private:
    BasicNode larg_;
    BasicNode rarg_;
    BinaryOperatorT type_;

public:
    BinaryOperator(BinaryOperatorT type, BasicNode const &larg, BasicNode const &rarg) :
    larg_(larg), rarg_(rarg), type_(type)
    {}

    BinaryOperator(BinaryOperatorT type, BasicNode&& larg, BasicNode&& rarg) :
    larg_(std::move(larg)), rarg_(std::move(rarg)), type_(type)
    {}

    BinaryOperator(BinaryOperatorT type, BasicNode const &larg, BasicNode&& rarg) :
    larg_(larg), rarg_(std::move(rarg)), type_(type)
    {}

    BinaryOperator(BinaryOperatorT type, BasicNode&& larg, BasicNode const &rarg) :
    larg_(std::move(larg)), rarg_(rarg), type_(type)
    {}

public:
    BinaryOperatorT type() const noexcept
    { return type_; }

    BasicNode const &larg() const & noexcept
    { return larg_; }
    BasicNode &larg() & noexcept
    { return larg_; }

    BasicNode const &rarg() const & noexcept
    { return rarg_; }
    BasicNode &rarg() & noexcept
    { return rarg_; }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class NumberLiteral final
{
private:
    int value_;
public:
    explicit NumberLiteral(int value) :
        value_(value)
    {}
public:
    int value() const noexcept
    { return value_; };

    NumberLiteral(const NumberLiteral& other) : value_(other.value_)
    {}

    NumberLiteral(NumberLiteral&& other): value_(other.value_)
    {}

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class StringLiteral final
{
private:
    std::string value_;
public:
    StringLiteral(std::string&& value) : /* string literal must own his value */
    value_(value)
    {}
public:
    std::string_view value() const & noexcept
    { return value_; }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

/* impl basic class for while, if, else-if classes */
class ConditionWithBody /* not final */
{
private:
    BasicNode condition_;
    BasicNode body_;
public:
    ConditionWithBody() = default;
    ConditionWithBody(BasicNode const &condition, BasicNode const &body) :
    condition_(condition), body_(body)
    {}

    ConditionWithBody(BasicNode const &condition, BasicNode &&body) :
    condition_(condition), body_(std::move(body))
    {}

    ConditionWithBody(BasicNode &&condition, BasicNode const &body) :
    condition_(std::move(condition)), body_(body)
    {}

    ConditionWithBody(BasicNode&& condition, BasicNode&& body) :
    condition_(std::move(condition)), body_(std::move(body))
    {}

public:
    BasicNode const &condition() const & noexcept
    { return condition_; }
    BasicNode &condition() & noexcept
    { return condition_; }

    BasicNode const &body() const & noexcept
    { return body_; }
    BasicNode &body() & noexcept
    { return body_; }

protected:
    mutable CodeLocation location_;
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class While final : public ConditionWithBody
{
public:
    using ConditionWithBody::ConditionWithBody;

private:
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class If final : public ConditionWithBody
{
public:
    using ConditionWithBody::ConditionWithBody;

public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Condition;

export
class Else final
{
private:
    BasicNode body_;
public:
    Else(BasicNode const &body) : body_(body)
    {}
    Else(BasicNode&& body) : body_(std::move(body))
    {}
public:
    BasicNode const &body() const & noexcept
    { return body_; }

    BasicNode &body() & noexcept
    { return body_; }

    friend class Condition;

private:
    Else() = default;

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Condition final
{
private:
    std::vector<BasicNode> ifs_;
    BasicNode else_;
public:
    Condition() = default;

    Condition(std::vector<BasicNode> const &ifs, BasicNode const & else_а_как_вот_это_назвать) :
       ifs_(ifs), else_(else_а_как_вот_это_назвать)
    {}

    Condition(std::vector<BasicNode>&& ifs, BasicNode&& else_а_как_вот_это_назвать) :
       ifs_(std::move(ifs)), else_(std::move(else_а_как_вот_это_назвать))
    {}

    Condition(std::vector<BasicNode> const & ifs, BasicNode&& else_а_как_вот_это_назвать) :
       ifs_(ifs), else_(std::move(else_а_как_вот_это_назвать))
    {}

    Condition(std::vector<BasicNode>&& ifs, BasicNode const & else_а_как_вот_это_назвать) :
       ifs_(std::move(ifs)), else_(else_а_как_вот_это_назвать)
    {}

public:
    void add_condition(BasicNode&& condition)
    { ifs_.push_back(condition); }

    void set_else(BasicNode&& else_а_как_вот_это_назвать)
    { else_ = std::move(else_а_как_вот_это_назвать); }

    bool has_else() const noexcept
    { return else_; }
public:

    std::vector<BasicNode> const &get_ifs() const & noexcept
    { return ifs_; }
    std::vector<BasicNode> &get_ifs() & noexcept
    { return ifs_; }

    BasicNode const &get_else() const & noexcept
    { return else_; }
    BasicNode &get_else() & noexcept
    { return else_; }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Return final
{
private:
    BasicNode expression_;
public:
    BasicNode const & expression() const & noexcept
    { return expression_; }
    BasicNode       & expression()       & noexcept
    { return expression_; }

    Return() = default;

    Return(BasicNode&& expression) :
        expression_(std::move(expression))
    {}
    Return(BasicNode const & expression) :
        expression_(expression)
    {}

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class FunctionDeclaration final
{
private:
    std::string name_;
    std::vector<std::string> args_;
    BasicNode body_;
private:
public:
    std::string_view name() const & noexcept
    { return name_; }

    std::vector<std::string> const & args() const & noexcept
    { return args_; } 

    std::vector<std::string>       & args()       & noexcept
    { return args_; } 

    BasicNode const & body() const & noexcept
    { return body_; }

    BasicNode       & body()       & noexcept
    { return body_; }

    FunctionDeclaration() = default;

FunctionDeclaration(std::string&& name, std::vector<std::string>&& args, BasicNode&& body) :
        name_(std::move(name)),
        args_(std::move(args)),
        body_(std::move(body))
    {}

    FunctionDeclaration(std::string&& name, std::vector<std::string>&& args, BasicNode const& body) :
        name_(std::move(name)),
        args_(std::move(args)),
        body_(body)
    {}

    FunctionDeclaration(std::string&& name, std::vector<std::string> const& args, BasicNode&& body) :
        name_(std::move(name)),
        args_(args),
        body_(std::move(body))
    {}

    FunctionDeclaration(std::string&& name, std::vector<std::string> const& args, BasicNode const& body) :
        name_(std::move(name)),
        args_(args),
        body_(body)
    {}

    void add_arg(std::string&& new_arg)
    { args_.push_back(std::move(new_arg)); }

    void set_name(std::string&& name)
    { name_ = std::move(name); }

    void set_body(BasicNode && new_body)
    { body_ = std::move(new_body); }

    void set_body(BasicNode const & new_body)
    { body_ = new_body; }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class FunctionCall final
{
private:
    std::string name_;
    std::vector<BasicNode> args_;
public:
    FunctionCall() = default;

    FunctionCall(std::string&& name, std::vector<BasicNode>&& args) :
        name_(std::move(name)), args_(std::move(args))
    {}

    FunctionCall(std::string&& name, std::vector<BasicNode> const& args) :
        name_(std::move(name)), args_(args)
    {}

    std::string_view name() const & noexcept
    { return name_; }

    std::vector<BasicNode> const & args() const & noexcept
    { return args_; }

    std::vector<BasicNode> & args() & noexcept
    { return args_; }

    void add_arg(BasicNode const & new_arg)
    { args_.push_back(new_arg); }

    void add_arg(BasicNode && new_arg)
    { args_.push_back(std::move(new_arg)); }

    void set_name(std::string&& name)
    { name_ = std::move(name); }

private:
    mutable CodeLocation location_;
public:
    CodeLocation & location() & noexcept
    { return location_; }

    CodeLocation const & location() const & noexcept
    { return location_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace last::node */
//--------------------------------------------------------------------------------------------------------------------------------------
