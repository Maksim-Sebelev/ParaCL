import thelast;
#include "create-basic-node.hpp"
CREATE_SAME(boost::json::value(), last::node::dumpable)

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>
#include <array>
#include <random>
#include <functional>
#include <fstream>
#include <sstream>

class NameTable
{
    std::vector<std::unordered_map<std::string, int>> scopes_;
public:
    NameTable()
    {
        enter_scope();
    }
    void enter_scope()
    {
        scopes_.emplace_back();
    }
    void exit_scope()
    {
        if (scopes_.size() > 1)
        {
            scopes_.pop_back();
        }
    }
    void declare_variable(std::string_view name, int val = 0)
    {
        if (name.empty())
            return;
        scopes_.back()[std::string(name)] = val;
    }
    bool is_declared(std::string_view name) const
    {
        if (name.empty())
            return false;
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
        {
            if (it->count(std::string(name)))
            {
                return true;
            }
        }
        return false;
    }
    std::optional<int> get_value(std::string_view name) const
    {
        if (name.empty())
            return std::nullopt;
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
        {
            auto found = it->find(std::string(name));
            if (found != it->end())
            {
                return found->second;
            }
        }
        return std::nullopt;
    }
    bool assign_value(std::string_view name, int new_value)
    {
        if (name.empty())
            return false;
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
        {
            auto found = it->find(std::string(name));
            if (found != it->end())
            {
                found->second = new_value;
                return true;
            }
        }
        return false;
    }
   
    size_t current_depth() const
    {
        return scopes_.size();
    }
    void reset()
    {
        scopes_.clear();
        enter_scope();
    }
};


struct NodeValue {
    last::node::BasicNode node;
    int value;
    std::string print_output; 
};


static std::mt19937 global_rng;
static std::ostringstream expected_output;

using NodeCreator = std::function<NodeValue(std::mt19937&, NameTable&)>;

static std::array<NodeCreator, 5> node_generators;


int compute_binary_op(last::node::BinaryOperator::BinaryOperatorT op, int left, int right)
{
    switch (op)
    {
        case last::node::BinaryOperator::AND:    return left && right;
        case last::node::BinaryOperator::OR:     return left || right;
        case last::node::BinaryOperator::ADD:    return left + right;
        case last::node::BinaryOperator::SUB:    return left - right;
        case last::node::BinaryOperator::MUL:    return left * right;
        case last::node::BinaryOperator::DIV:    return right != 0 ? left / right : 0;
        case last::node::BinaryOperator::REM:    return right != 0 ? left % right : 0;
        case last::node::BinaryOperator::ISAB:   return left > right;
        case last::node::BinaryOperator::ISABE:  return left >= right;
        case last::node::BinaryOperator::ISLS:   return left < right;
        case last::node::BinaryOperator::ISLSE:  return left <= right;
        case last::node::BinaryOperator::ISEQ:   return left == right;
        case last::node::BinaryOperator::ISNE:   return left != right;
        default: return 0;
    }
}

void initialize_generators()
{
    node_generators[0] = 
        [](auto& rng, auto& table) -> NodeValue
    {
        int value = std::uniform_int_distribution<int>{-100, 100}(rng);
        return {
            last::node::create(last::node::NumberLiteral{value}),
            value,
            ""
        };
    };
    
    node_generators[1] = 
        [](auto& rng, auto& table) -> NodeValue
    {
        std::string strLiteral{"Text"};
        for (size_t i = 0, factor = std::uniform_int_distribution<size_t>{0, 2}(rng);
            i < factor; ++i)
        {
            strLiteral += strLiteral;
        }
        return {
            last::node::create(last::node::StringLiteral{std::move(strLiteral)}),
            0,
            ""
        };
    };
    
    node_generators[2] = 
        [](auto& rng, auto& table) -> NodeValue
    {
        std::string var_name = "var_" + std::to_string(
            std::uniform_int_distribution<int>{0, 2}(rng)
        );
        
        int value = 0;
        if (table.is_declared(var_name))
        {
            value = table.get_value(var_name).value_or(0);
        }
        else
        {
            value = std::uniform_int_distribution<int>{1, 50}(rng);
            table.declare_variable(var_name, value);
        }
        
        return {
            last::node::create(last::node::Variable{var_name}),
            value,
            ""
        };
    };
    
    node_generators[3] = 
        [](auto& rng, auto& table) -> NodeValue
    {
        
        std::array<last::node::BinaryOperator::BinaryOperatorT, 13> valid_ops = {
            last::node::BinaryOperator::AND,
            last::node::BinaryOperator::OR,
            last::node::BinaryOperator::ADD,
            last::node::BinaryOperator::SUB,
            last::node::BinaryOperator::MUL,
            last::node::BinaryOperator::DIV,
            last::node::BinaryOperator::REM,
            last::node::BinaryOperator::ISAB,
            last::node::BinaryOperator::ISABE,
            last::node::BinaryOperator::ISLS,
            last::node::BinaryOperator::ISLSE,
            last::node::BinaryOperator::ISEQ,
            last::node::BinaryOperator::ISNE
        };
        
        auto op = valid_ops[std::uniform_int_distribution<size_t>{0, 12}(rng)];
        
        auto left_nv = node_generators[0](rng, table);  
        auto right_nv = node_generators[0](rng, table); 
        
        int result = compute_binary_op(op, left_nv.value, right_nv.value);
        
        return {
            last::node::create(
                last::node::BinaryOperator{op, std::move(left_nv.node), std::move(right_nv.node)}
            ),
            result,
            ""
        };
    };
    
    node_generators[4] = 
        [](auto& rng, auto& table) -> NodeValue
    {
        std::vector<last::node::BasicNode> args;
        std::ostringstream print_stream;
        size_t num_args = std::uniform_int_distribution<size_t>{1, 3}(rng);
        
        for (size_t i = 0; i < num_args; ++i)
        {
            size_t arg_type = std::uniform_int_distribution<size_t>{0, 2}(rng);
            NodeValue arg_nv;
            
            if (arg_type == 0 || arg_type == 1)
            {
                arg_nv = node_generators[arg_type](rng, table); 
            }
            else
            {
                arg_nv = node_generators[2](rng, table); 
            }
            
            args.emplace_back(std::move(arg_nv.node));
            print_stream << arg_nv.value;
            if (i < num_args - 1) print_stream << " ";
        }
        
        print_stream << "\n";
        
        return {
            last::node::create(last::node::Print{std::move(args)}),
            0,
            print_stream.str()
        };
    };
}


NodeValue generate_assignment(std::mt19937& rng, NameTable& table)
{
    
    std::string var_name = "var_" + std::to_string(
        std::uniform_int_distribution<int>{0, 2}(rng)
    );
    
    if (!table.is_declared(var_name))
    {
        table.declare_variable(var_name, 0);
    }
    
    
    size_t right_type = std::uniform_int_distribution<size_t>{0, 3}(rng);
    NodeValue right_nv = node_generators[right_type](rng, table);
    
    
    int new_value = right_nv.value;
    table.assign_value(var_name, new_value);
    
    auto left_node = last::node::create(last::node::Variable{var_name});
    
    return {
        last::node::create(
            last::node::BinaryOperator{
                last::node::BinaryOperator::ASGN,
                std::move(left_node),
                std::move(right_nv.node)
            }
        ),
        new_value,
        ""
    };
}

last::AST generateAST()
{
    std::random_device rd;
    global_rng.seed(rd());
    
    NameTable table;
    initialize_generators();
    
    expected_output.str("");
    expected_output.clear();
    
    auto generateStatements = [&table]() -> std::vector<last::node::BasicNode>
    {
        std::vector<last::node::BasicNode> statements;
        std::uniform_int_distribution<size_t> distrib(3, 8);
        size_t numberOfStatements = distrib(global_rng);
        
        
        for (size_t i = 0; i < 3; ++i)
        {
            std::string var_name = "var_" + std::to_string(i);
            int init_value = std::uniform_int_distribution<int>{1, 50}(global_rng);
            table.declare_variable(var_name, init_value);
        }
        
        
        for (size_t i = 0; i < numberOfStatements; ++i)
        {
            NodeValue stmt_nv;
            
            size_t stmt_type = std::uniform_int_distribution<size_t>{0, 100}(global_rng);
            
            if (stmt_type < 30)  
            {
                stmt_nv = generate_assignment(global_rng, table);
            }
            else if (stmt_type < 60)  
            {
                stmt_nv = node_generators[4](global_rng, table);
                expected_output << stmt_nv.print_output;
            }
            else if (stmt_type < 80)  
            {
                stmt_nv = node_generators[3](global_rng, table);
            }
            else  
            {
                size_t expr_type = std::uniform_int_distribution<size_t>{0, 2}(global_rng);
                stmt_nv = node_generators[expr_type](global_rng, table);
            }
            
            statements.emplace_back(std::move(stmt_nv.node));
        }
        
        return statements;
    };
    
    return last::node::create(last::node::Scope(std::move(generateStatements())));
}

void write_expected_output(const std::string& filename)
{
    std::ofstream file(filename);
    file << expected_output.str();
    file.close();
}

int main(int argc, char** argv) try
{
    auto&& program = generateAST();
    
    
    last::write(program, "a.out");
    
    
    write_expected_output("expected_output.txt");
    
    std::cout << "AST successfully generated and written to a.out" << std::endl;
    std::cout << "Expected output written to expected_output.txt" << std::endl;
    
    return 0;
}
catch(const std::exception& e)
{
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
}