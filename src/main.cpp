#include <cstdlib>
#include <iostream>
#include <exception>
#include "Shell.h"

int main() try
{
    Shell().run();
    return EXIT_SUCCESS;
}
catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
    return EXIT_FAILURE;
}
catch (...) {
    std::cerr << "Unknown exception\n";
    return EXIT_FAILURE;
}