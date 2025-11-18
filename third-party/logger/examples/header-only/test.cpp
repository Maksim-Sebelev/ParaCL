#include "pineaplog.hpp"

// int main()
// {
//     PineapLog::glog.log("Hello header-only PineapLog!");

//     return 0;
// }

int main()
{
PineapLog::Logger log("my-first-logger", "/home/ananasik/projects/C++/logger/2_version/examples/header-only/build/ananas.jpg", true);
log.logc(PineapLog::LogColor::Pink, "My first record in PineapLogger!");

log.log("This text is pink, because color saved by last usage");

log.title("BLUE TITLE, big text", PineapLog::LogColor::Blue);

log.logc(PineapLog::LogColor::White, "Hmm, I want to know, from where i'm writing it!");
log.code_place();
log.log("so, now I know it! method code_place - save last color!");

log.log_endl(/*Now I want to skip line in html*/);

log.logc(PineapLog::LogColor::Blue, "And if I want to make a lot records in 1 line?");
log.logc_in_line_begin(PineapLog::LogColor::Red);
for (int i = 0; i < 5; i++)
    log.log_in_line(i, " ");
log.log_in_line_end();
return 0;
}