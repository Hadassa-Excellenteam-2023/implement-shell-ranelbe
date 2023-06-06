#include "Shell.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>


/*
* run the shell by reading input from the user and executing commands.
*/
void Shell::run()
{
	std::string input;
    std::cout << getCurrentDirectory() << "# ";
	while (std::getline(std::cin, input))
	{
        cleanUpBackgroundJobs();
        std::vector<std::string> tokens = extractTokens(input);

        if (tokens.size() != 0) { // command is not empty

            try {
                //this cases must be handled by the shell itself
                if (tokens[0] == "cd") {
                    changeDirectory(tokens);
                }
                else if (tokens[0] == "myjobs") {
                    showBackgroundJobs();
                }
                else if (tokens[0] == "exit") {
                    break;
                }
                else {
                    if (tokens.back() == "&") {
                        tokens.pop_back(); // remove the "&"
                        executeCommand(tokens, true); // execute in background
                    }
                    else {
                        executeCommand(tokens, false); // execute in foreground
                    }
                }

            } catch (const std::exception& e) {
                std::cerr << e.what();
            }
        } 

        std::cout << getCurrentDirectory() << "# "; // ready for the next input
	}
}


/*
* get the current directory.
* @return the current directory as string.
*/
std::string Shell::getCurrentDirectory() const
{
    char cwd[PATH_MAX];
    return (getcwd(cwd, sizeof(cwd)) != nullptr) ?
        std::string(cwd) : ERROR_UNKNOWN_DIR;
}


/*
* change the current directory.
* @param tokens - the command and its arguments.
*/
void Shell::changeDirectory(const std::vector<std::string>& tokens) const
{
    if (tokens.size() < 2) {
        throw std::runtime_error(ERROR_UNSPECIFIED_DIR);
    }
    std::string directory = tokens[1];
    if (chdir(directory.c_str()) != 0) {
        throw std::runtime_error(ERROR_CHANGE_DIR);
    }
}

/*
* split the input string into tokens - the command and its arguments.
* @param input - the input string to split.
* @return a vector of tokens.
*/
std::vector<std::string> Shell::extractTokens(const std::string& input) const
{
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}


/*
* execute the command if it exists.
* @param tokens - the command and its arguments.
* @param isBackground - whether the command should be executed in the background.
* @throw runtime_error in case of an error.
*/
void Shell::executeCommand(const std::vector<std::string>& tokens, bool isBackground)
{
   std::string command = tokens[0];

   //look for the executable file path
   std::string executablePath = findExecutablePath(command); 
   if (executablePath.empty()) {
       throw std::runtime_error(command + ": " + ERROR_COMMAND_NOT_FOUND);
   }

   // command found, create a child process to execute it
   pid_t pid = fork();
   if (pid == -1) {
       throw std::runtime_error(ERROR_CREATE_CHILD);
   }
    
   // child process
   if (pid == 0) {
       executeChild(executablePath, tokens);
       throw std::runtime_error(command + ": " + ERROR_EXECUTE_COMMAND);
   }

   // parent process
   if (isBackground) {
       m_jobs[pid] = command; // add the  process to the jobs list
   } else {
       waitpid(pid, nullptr, 0); // parent waits for child to finish
   }
}


/*
* find the path of the executable file of the command.
* @param command - the command to find.
* @return the path of the executable file or an empty string if path not found.
*/
std::string Shell::findExecutablePath(const std::string& command) const
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


/*
* execute the command in the child process via execvp.
* @param executablePath - the path of the executable file.
* @tokens - the command and its arguments.
*/
void Shell::executeChild(const std::string& executablePath, const std::vector<std::string>& tokens)
{
    // convert vector of strings to vector of char* so it can be passed to execvp
    std::vector<char*> args(tokens.size() + 1);
    std::transform(tokens.begin(), tokens.end(), args.begin(),
        [](const std::string& token) {
            return const_cast<char*>(token.c_str());
        });
    args[tokens.size()] = nullptr; //must be null terminated
    execvp(executablePath.c_str(), args.data()); // execute the command
}


/*
* clean up the jobs that have finished running.
*/
void Shell::cleanUpBackgroundJobs()
{
    for (auto it = m_jobs.cbegin(); it != m_jobs.cend();)
    {
        pid_t result = waitpid(it->first, nullptr, WNOHANG);
        if (result > 0) {
            m_jobs.erase(it++);
        } else {
            ++it;
        }
    }
}


/*
* show the jobs that are currently running.
* display the PID, command and status of each job.
* kill jobs that have finished running.
*/
void Shell::showBackgroundJobs() const
{
    for (const auto& job : m_jobs) {
        std::cout << "PID: " << job.first << ", Command: "
            << job.second << ", Status: Running\n";
    }
}
