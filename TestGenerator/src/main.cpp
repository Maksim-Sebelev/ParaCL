#include <cstdlib>
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
#include <iostream>

// #include "create-basic-node.hpp"

import thelast;
import ast_serializer;
import test_generator;

// CREATE_SAME(last::node::writable, last::node::dumpable, last::node::serializable)



int main(int argc, char** argv) try
{
    auto&& program = test_generator::generate_random_ast();
    
    last::write(program, "generated.ast.json");
    test_generator::ast_serialize::serialize(program, "generated.cl");

    test_generator::write_expected_output("expected_output.ans");
   
    std::cout << "AST successfully generated and written to a.out" << std::endl;
    std::cout << "Expected output written to expected_output.txt" << std::endl;

    std::system("./paracli generated.cl && echo \"=============\" && cat expected_output.ans");

    return 0;
}
catch(const std::exception& e)
{
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
}
catch(...)
{
    std::cerr << "undefined exception catched\n";
    return 1;
}
