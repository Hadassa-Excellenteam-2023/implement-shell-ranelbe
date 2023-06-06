#include "InputParser.h"
#include <sstream>


/*
* split the input string into tokens
* @param input - the input string to split.
* @return a vector of tokens.
*/
std::vector<std::string> InputParser::tokenize(const std::string& input)
{
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
