module;

#include <filesystem>

export module options;


namespace ParaCL::options
{

export
enum class OptimizeLevel
{
    O0, O1, O2, O3
};

export
struct Options
{
    std::filesystem::path tmp_directory = "/tmp";

    std::filesystem::path input_file;
    std::filesystem::path output_file = "a.out";
    std::filesystem::path tmp_ir_file = "a.ll";

    OptimizeLevel optimize_level = OptimizeLevel::O3;

    bool debug      : 1 = false;
    bool ast_dump   : 1 = false;
    bool save_temps : 1 = false;

    void correct_configure()
    {
        tmp_ir_file = input_file.filename();
        tmp_ir_file.replace_extension(".ll");

        if (not save_temps)
        {
            tmp_ir_file = tmp_directory / tmp_ir_file;
        }
    }
};

} /* namespace ParaCL::options */