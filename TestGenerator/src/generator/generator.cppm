module;

#include <string>
#include <cassert>
#include <cstddef>
#include <random>

#include "create-basic-node.hpp"

export module test_generator;

//---------------------------------------------------------------------------------------------------------------

export import thelast;
import name_generator;
import ast_serializer;
import global_create_settings;
import name_inserter;

//---------------------------------------------------------------------------------------------------------------
namespace test_generator
{
//---------------------------------------------------------------------------------------------------------------


/*

Global plan:

1) generate program without names
2) set names

*/

class Context
{
private:


};

class ASTBuilder
{

public:
    last::node::BasicNode generate_new_variable();
    last::node::BasicNode generate_existing_variable(unique_name_id_t id);
};


class AstGenerator
{
private:
    using NodeCreaterSignature = last::node::BasicNode(std::mt19937&, nametable::Nametable&);
    using NodeCreator = std::function<NodeCreaterSignature>;

private:
    std::mt19937 random_;
    last::node::Scope root_;
    name_generator::NameGenerator name_generator_;

private:


    void initialize_names()
    {
        name_inserter::in
    }
 
    last::node::BasicNode generate_random_statement()

public:
    AstGenerator() :
    name_generator_(random_)
    {
    }

    AST generate_random_ast(size_t number_statements);

    std::string get_correct_output() const
    { return correct_output_.str(); }
};

//---------------------------------------------------------------------------------------------------------------

void AstGenerator::initialize_names()
{
    for (auto&& statement: root_)
    {
        if (stast)
    }
}

//---------------------------------------------------------------------------------------------------------------


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


enum NodeID
{
    NumberLiteral = 0,
    StringLiteral,
    Variable,
    BinaryOperator,
    Print,
};

void initialize_generators()
{
    node_generators[0] = 
        [](auto& rng, auto& nametable) -> last::node::BasicNode
    {
        int value = std::uniform_int_distribution<int>{-100, 100}(rng);
        return {
            last::node::create(last::node::NumberLiteral{value}),
            value,
            ""
        };
    };

    node_generators[1] = 
        [](auto& rng, auto& nametable) -> last::node::BasicNode
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
        [](auto& rng, auto& nametable) -> last::node::BasicNode
    {
        std::string var_name = "var_" + std::to_string(
            std::uniform_int_distribution<int>{0, 2}(rng)
        );
        
        int value = 0;
        if (nametable.exists(var_name))
        {
            value = nametable.get_variable_value(var_name);
        }
        else
        {
            value = std::uniform_int_distribution<int>{1, 50}(rng);
            nametable.set_value(var_name, value);
        }
        
        return {
            last::node::create(last::node::Variable{std::move(var_name)}),
            value,
            ""
        };
    };
    
    node_generators[3] = 
        [](auto& rng, auto& nametable) -> last::node::BasicNode
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
        
        auto left_nv = node_generators[0](rng, nametable);  
        auto right_nv = node_generators[0](rng, nametable); 
        
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
        [](auto& rng, auto& nametable) -> last::node::BasicNode
    {
        std::vector<last::node::BasicNode> args;
        std::ostringstream print_stream;
        size_t num_args = std::uniform_int_distribution<size_t>{1, 3}(rng);
        
        for (size_t i = 0; i < num_args; ++i)
        {
            size_t arg_type = std::uniform_int_distribution<size_t>{0, 2}(rng);
            last::node::BasicNode arg_nv;
            
            if (arg_type == 0 || arg_type == 1)
            {
                arg_nv = node_generators[arg_type](rng, nametable); 
            }
            else
            {
                arg_nv = node_generators[2](rng, nametable); 
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

NodeValue generate_assignment(std::mt19937& rng, nametable::Nametable& nametable)
{
    std::string var_name = "var_" + std::to_string(
        std::uniform_int_distribution<int>{0, 2}(rng)
    );
    
    nametable.set_value(var_name, 0);

    size_t right_type = std::uniform_int_distribution<size_t>{0, 3}(rng);
    last::node::BasicNode right_nv = node_generators[right_type](rng, nametable);
    
    
    int new_value = right_nv.value;
    nametable.set_value(var_name, new_value);
    
    auto left_node = last::node::create(last::node::Variable{std::move(var_name)});

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

export
last::AST generate_random_ast()
{
    std::random_device rd;
    global_rng.seed(rd());
    
    nametable::Nametable nametable; nametable.new_scope(); /* global scope */
    initialize_generators();
    
    expected_output.str("");
    expected_output.clear();
    
    auto generateStatements = [&nametable]() -> std::vector<last::node::BasicNode>
    {
        std::vector<last::node::BasicNode> statements;
        std::uniform_int_distribution<size_t> distrib(3, 8);
        size_t numberOfStatements = distrib(global_rng);
        
        
        for (size_t i = 0; i < 3; ++i)
        {
            std::string var_name = "var_" + std::to_string(i);
            int init_value = std::uniform_int_distribution<int>{1, 50}(global_rng);
            nametable.set_value(var_name, init_value);
        }
        
        
        for (size_t i = 0; i < numberOfStatements; ++i)
        {
            last::node::BasicNode stmt_nv;

            size_t stmt_type = std::uniform_int_distribution<size_t>{0, 100}(global_rng);

            if (stmt_type < 30)  
            {
                stmt_nv = generate_assignment(global_rng, nametable);
            }
            else if (stmt_type < 60)  
            {
                stmt_nv = node_generators[4](global_rng, nametable);
                expected_output << stmt_nv.print_output;
            }
            else if (stmt_type < 80)  
            {
                stmt_nv = node_generators[3](global_rng, nametable);
            }
            else  
            {
                size_t expr_type = std::uniform_int_distribution<size_t>{0, 2}(global_rng);
                stmt_nv = node_generators[expr_type](global_rng, nametable);
            }
            
            statements.emplace_back(std::move(stmt_nv.node));
        }
        
        return statements;
    };
    
    return last::node::create(last::node::Scope(std::move(generateStatements())));
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator */
//---------------------------------------------------------------------------------------------------------------
