#include "DirectoryManager.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>


/*
* get the current directory.
* @return the current directory as string.
*/
std::string DirectoryManager::getCurrentDirectory()
{
    char cwd[PATH_MAX];
    return (getcwd(cwd, sizeof(cwd)) != nullptr) ?
        std::string(cwd) : ERROR_UNKNOWN_DIR;
}


/*
* change the current directory path.
* @param args - the command and its arguments.
*/
void DirectoryManager::changeDirectory(const std::string& newPath)
{
    if (newPath.empty()) {
        throw std::runtime_error(ERROR_UNSPECIFIED_DIR);
    }
    if (chdir(newPath.c_str()) != 0) {
        throw std::runtime_error(ERROR_CHANGE_DIR);
    }
}


/*
* find the path of the executable file of the command.
* @param command - the command to find.
* @return the path of the executable file or an empty string if path not found.
*/
std::string DirectoryManager::findExecutablePath(const std::string& command)
{
    // check if command is an absolute path
    if (access(command.c_str(), X_OK) == 0) {
        return command;
    }

    // look in the PATH environment variable for the command
    std::string pathEnv(getenv("PATH"));
    std::stringstream pathStream(pathEnv);
    std::string dir;

    while (std::getline(pathStream, dir, ':')) {
        std::string executable = dir + "/" + command;
        if (access(executable.c_str(), X_OK) == 0) {
            return executable;
        }
    }
    return ""; //bummer command not found, the path is empty
}
