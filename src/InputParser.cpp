#include "InputParser.h"
#include "RedirectionsManager.h"
#include <sstream>


/*
* split the input string into commands.
* @param input - the input string to split.
* @return commands - vector of commands, 
* where each command is a vector of tokens + isBackground flag.
*/
commands InputParser::tokenize(const std::string& input)
{
    std::vector<Command> commands;
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        auto redirectionSymbol = m_redirectionSymbols.find(token);
        if (redirectionSymbol != m_redirectionSymbols.end()) {
            RedirectionsManager::instance().reset();
            int stdFd = redirectionSymbol->second.first;
            int flags = redirectionSymbol->second.second;

            std::string filename;
            if (!(ss >> filename)) {
				throw std::runtime_error(ERROR_MISSING_FILENAME);
			}
            RedirectionsManager::instance().redirectIO(filename, flags, stdFd);
        } 
        else if (token == "|") {
            bool isBackground = (tokens.back() == "&");
            if(isBackground) tokens.pop_back();
            commands.push_back(Command(tokens, isBackground));
            tokens.clear();
        }
        else {
            tokens.push_back(token);
        }
    }

    if (!tokens.empty()) {
        bool isBackground = (tokens.back() == "&");
        if (isBackground) tokens.pop_back();
        commands.push_back(Command(tokens, isBackground));
    }

    return commands;
}
