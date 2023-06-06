#include "Shell.h"
#include <iostream>
#include <cstdlib>
#include <exception>


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