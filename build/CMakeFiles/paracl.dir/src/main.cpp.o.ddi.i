# 0 "/home/matvey/work/ParaCL/src/main.cpp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "/home/matvey/work/ParaCL/src/main.cpp"
import  options_parser;
import  run_paracl;
import  parse_paracl_exit_code;



int main(int argc, char* argv[])
{
    const OptionsParsing::program_options_t program_options = OptionsParsing::parse_program_options(argc, argv);
    const int paracl_exit_code = ParaCL ::run_paracl (program_options);

    return ParaCL::parse_paracl_exit_code(argv[0], paracl_exit_code);
}
