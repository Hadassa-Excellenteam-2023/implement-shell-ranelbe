#include "Shell.h"
#include "DirectoryManager.h"
#include "JobsManager.h"
#include "InputParser.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>


/*
* run the shell by reading input from the user and executing commands.
*/
void Shell::run()
{
	std::string input;
    std::cout << DirectoryManager::getCurrentDirectory() << "# ";
	while (std::getline(std::cin, input))
	{
        JobsManager::instance().cleanUpBackgroundJobs();
        std::vector<std::string> tokens = InputParser::tokenize(input);

        if (tokens.size() != 0) { // command is not empty

            try {
                //this cases must be handled by the shell itself
                if (tokens[0] == "cd") {
                    DirectoryManager::changeDirectory((tokens.size() > 1) ? tokens[1] : "");
                } 
                else if (tokens[0] == "myjobs") {
                    JobsManager::instance().showBackgroundJobs();
                } 
                else if (tokens[0] == "exit") {
                    break;
                } 
                else {
                    if (tokens.back() == "&") {
                        tokens.pop_back(); // remove the "&"
                        executeCommand(tokens, true); // execute in background
                    } else {
                        executeCommand(tokens, false); // execute in foreground
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << e.what();
            }
        } 

        std::cout << DirectoryManager::getCurrentDirectory() << "# "; // ready for the next input
	}
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
   std::string executablePath = DirectoryManager::findExecutablePath(command);
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
       throw std::runtime_error(command + ": " + ERROR_EXECUTE_COMMAND); // execvp failed
   }

   // parent process
   if (isBackground) {
       JobsManager::instance().addJob(pid, command); // add the  process to the jobs list
   } else {
       waitpid(pid, nullptr, 0); // parent waits for child to finish
   }
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
