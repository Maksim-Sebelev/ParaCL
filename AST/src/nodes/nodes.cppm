module;

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

export module ast_nodes;

export import node_type_erasure;

namespace last::node
{





// --- Scope ---------------------------------------------------------------------------------------

export class Scope final : private std::vector<BasicNode>
{
public:
    using std::vector<BasicNode>::emplace_back;
    using std::vector<BasicNode>::push_back;
    using std::vector<BasicNode>::begin;
    using std::vector<BasicNode>::end;
    using std::vector<BasicNode>::size;

    Scope() = default;
    explicit Scope(size_t size) : std::vector<BasicNode>(size) {}
    explicit Scope(std::vector<BasicNode>&& nodes) : std::vector<BasicNode>(std::move(nodes)) {}
    Scope(std::initializer_list<BasicNode> il) : std::vector<BasicNode>(il) {}
};




// --- Variable ------------------------------------------------------------------------------------

export
class Variable final
{
    std::string name_;

public:
    explicit Variable(std::string&& name) : name_(std::move(name)) {}

    [[nodiscard]] std::string_view name() const & noexcept { return name_; }
};




// --- Print ---------------------------------------------------------------------------------------

export class Print final : private std::vector<BasicNode>
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

    Print() = default;
    explicit Print(std::vector<BasicNode>&& args) : std::vector<BasicNode>(std::move(args)) {}
    Print(std::initializer_list<BasicNode> il) : std::vector<BasicNode>(il) {}
};





// --- Scan ----------------------------------------------------------------------------------------

export class Scan final
{
public:
    Scan() = default;
};





// --- UnaryOperator -------------------------------------------------------------------------------

export class UnaryOperator final
{
public:
    enum UnaryOperatorT { MINUS, PLUS, NOT };

private:
    BasicNode      arg_;
    UnaryOperatorT type_;

public:
    UnaryOperator(UnaryOperatorT type, const BasicNode& arg) 
        : arg_(arg), type_(type) {}

    UnaryOperator(UnaryOperatorT type, BasicNode&& arg) 
        : arg_(std::move(arg)), type_(type) {}

    [[nodiscard]] UnaryOperatorT    type() const noexcept { return type_; }
    [[nodiscard]] const BasicNode& arg()  const & noexcept { return arg_; }
};





// --- BinaryOperator ------------------------------------------------------------------------------

export class BinaryOperator final
{
public:
    enum BinaryOperatorT {
        AND, OR, ADD, SUB, MUL, DIV, REM,
        ISAB, ISABE, ISLS, ISLSE, ISEQ, ISNE,
        ASGN, ADDASGN, SUBASGN, MULASGN, DIVASGN, REMASGN
    };

private:
    BasicNode       larg_;
    BasicNode       rarg_;
    BinaryOperatorT type_;

public:
    template <typename TARG>
        requires std::is_constructible_v<BasicNode, TARG>
    BinaryOperator(BinaryOperatorT type, TARG&& larg, TARG&& rarg)
        : larg_(std::forward<TARG>(larg)), rarg_(std::forward<TARG>(rarg)), type_(type) {}

    [[nodiscard]] BinaryOperatorT  type() const noexcept { return type_; }
    [[nodiscard]] const BasicNode& larg() const & noexcept { return larg_; }
    [[nodiscard]] const BasicNode& rarg() const & noexcept { return rarg_; }
};





// --- Literals ------------------------------------------------------------------------------------

export class NumberLiteral final {
    int value_;

public:
    explicit NumberLiteral(int value) : value_(value) {}
    [[nodiscard]] int value() const noexcept { return value_; }
};

export class StringLiteral final {
    std::string value_;

public:
    explicit StringLiteral(std::string&& value) : value_(std::move(value)) {}
    [[nodiscard]] std::string_view value() const & noexcept { return value_; }
};





class ConditionWithBody
{
    BasicNode cond_;
    BasicNode body_;

public:
    ConditionWithBody() = default;
    
    template<typename T>
        requires std::is_constructible_v<BasicNode, T>
    ConditionWithBody(T&& cond, T&& body)
        : cond_(std::forward<T>(cond)), body_(std::forward<T>(body)) {}

    [[nodiscard]] const BasicNode& condition() const & noexcept { return cond_; }
    [[nodiscard]] const BasicNode& body()      const & noexcept { return body_; }
};

export class While final : public ConditionWithBody {
public:
    using ConditionWithBody::ConditionWithBody;
};

export class If final : public ConditionWithBody {
public:
    using ConditionWithBody::ConditionWithBody;
};

export class Else final {
    BasicNode body_;

public:
    explicit Else(const BasicNode& body) : body_(body) {}
    explicit Else(BasicNode&& body)      : body_(std::move(body)) {}

    [[nodiscard]] const BasicNode& body() const & noexcept { return body_; }

    friend class Condition;
private:
    Else() = default;
};






export class Condition final {
    std::vector<BasicNode> ifs_;
    BasicNode              else_node_;

public:
    Condition() = default;

    Condition(std::vector<BasicNode>&& ifs, BasicNode&& else_node)
        : ifs_(std::move(ifs)), else_node_(std::move(else_node)) {}

    void add_condition(BasicNode&& cond) { ifs_.push_back(std::move(cond)); }
    void set_else(BasicNode&& else_node)  { else_node_ = std::move(else_node); }

    [[nodiscard]] bool has_else() const noexcept { return static_cast<bool>(else_node_); }

    [[nodiscard]] const std::vector<BasicNode>& get_ifs()  const & noexcept { return ifs_; }
    [[nodiscard]] const BasicNode&              get_else() const & noexcept { return else_node_; }
};

} // namespace last::node
