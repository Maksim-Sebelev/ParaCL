import thelast;
import name_generator;

#include <random>

int main()
{
    std::mt19937 rng;
    auto&& ng = test_generator::name_generator::NameGenerator(rng);

    auto&& ast = last::AST{ng.ge};
    return 0;
}
