#include "lexer.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <cstdio>

extern FILE* yyin;
extern int yyparse();

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* input_file = fopen(argv[1], "r");
        if (input_file) {
            yyin = input_file;
        } else {
            std::cerr << "Cannot open file: " << argv[1] << std::endl;
            return 1;
        }
    } else {
        std::cout << "Enter your program (Ctrl+D to end):" << std::endl;
        yyin = stdin;
    }

    int result = yyparse();
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return result;
}
