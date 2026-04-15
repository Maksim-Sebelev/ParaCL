#include <cstdlib>
#include <exception>
#include <iostream>
#include <filesystem>

import read_options;
import frontend;
import backend;

namespace ParaCL
{
void remove_temps(ParaCL::options::Options const & options)
{
    if (options.save_temps) return;
    std::filesystem::remove(options.tmp_ir_file);
}
} /* namespace ParaCL */

int main(int argc, char* argv[]) try
{
    auto&& options = ParaCL::options::read_options(argc, argv);

    ParaCL::frontend::run(options);
    ParaCL::backend ::run(options);

    ParaCL::remove_temps(options);

    return EXIT_SUCCESS;
}
catch (std::exception const & e)
{
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "Undefined error.\nPlease submit a but report with your code.\n";
    return EXIT_FAILURE;
}
