module;

#include <stdexcept>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

export module backend;

import options;

namespace ParaCL::backend
{

export
void run(options::Options const & options)
{
    auto&& command = std::ostringstream{};

    command << "clang " << options.tmp_ir_file;

    if (options.save_temps)
    {
        command << " --save-temps";
    }

    if (options.debug)
    {
        command << " -O0 -g -fsanitize=address -fsanitize=undefined -fstack-protector-all";
    }
    else
    {
        switch (options.optimize_level)
        {
            case options::OptimizeLevel::O0: command << " -O0"; break;
            case options::OptimizeLevel::O1: command << " -O1"; break;
            case options::OptimizeLevel::O2: command << " -O2"; break;
            case options::OptimizeLevel::O3: command << " -O3"; break;
            default:
#if defined(NDEBUG)
                __builtin_unreachable();
#else /* defined(NDEBUG) */
                throw std::runtime_error("undefind optimize level");
#endif /* defined(NDEBUG) */
        }
    }

    command << " -o " << options.output_file;

    auto&& result = std::system(command.str().c_str());

    if (result == EXIT_SUCCESS) return;

    throw std::runtime_error("compilation failed with exit code " + std::to_string(result));
}

} /* namespace ParaCL::backend */
