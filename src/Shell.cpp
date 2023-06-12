#include "Shell.h"
#include "DirectoryManager.h"
#include "JobsManager.h"
#include "InputParser.h"
#include "RedirectionsManager.h"
#include "Pipes.h"
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
        try {
            JobsManager::instance().cleanUpBackgroundJobs();
            commands commands = InputParser::instance().tokenize(input);
            if (!commands.empty()) { // command is not empty

                std::vector<std::string> tokens = commands[0].tokens;
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
                    execute(commands); // execute in background
                }
            } 
        } catch (const std::exception& e) {
            std::cerr << e.what();
        }
        RedirectionsManager::instance().reset(); // reset redirections
        std::cout << DirectoryManager::getCurrentDirectory() << "# "; // ready for the next input
	}
}


/*
* execute, handle both single command / piped commands.
* @param commands: the commands to be executed.
*/
void Shell::execute(const commands& commands)
{
    if (commands.size() == 1) {
        executeSingleCommand(commands[0]);
    }
    else {
        executePipedCommand(commands);
    }
}

/*
* excecute a single command.
* @param command: the command to be executed.
*/
void Shell::executeSingleCommand(const Command& command)
{
    std::string commandName = command.tokens[0];

    //look for the executable file path
    std::string executablePath = DirectoryManager::findExecutablePath(commandName);
    if (executablePath.empty()) {
        throw std::runtime_error(commandName + ": " + ERROR_COMMAND_NOT_FOUND);
    }

    // command found, create a child process to execute it
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error(ERROR_CREATE_CHILD);
    }

    // child process
    if (pid == 0) {
        executeChild(executablePath, command.tokens);
        throw std::runtime_error(commandName + ": " + ERROR_EXECUTE_COMMAND); // execvp failed
    }

    // parent process
    if (command.isBackground) {
        JobsManager::instance().addJob(pid, commandName); // add the  process to the jobs list
    }
    else {
        waitpid(pid, nullptr, 0); // parent waits for child to finish
    }
}


/*
* execute piped commands.
* @param commands: the commands to be executed.
*/
void Shell::executePipedCommand(const commands& commands)
{
    int pipeCount = commands.size() - 1;
    Pipes pipes(pipeCount);

    for (int i = 0; i < commands.size(); ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            throw std::runtime_error(ERROR_CREATE_CHILD);
        }
        else if (pid == 0) {
            if (i != 0) {
                dup2(pipes.getInputPipeFD(i - 1), STDIN_FILENO);
            }
            if (i != commands.size() - 1) {
                dup2(pipes.getOutputPipeFD(i), STDOUT_FILENO);
            }
            pipes.closePipes();
            executeSingleCommand(commands[i]);
            exit(0);
        }
    }
    pipes.closePipes();
    for (int i = 0; i < commands.size(); ++i) {
        int status;
        wait(&status);
    }
}


/*
* execute the command in the child process via execvp.
* @param executablePath - the path of the executable file.
* @tokens - the command and its arguments.
*/
void Shell::executeChild(const std::string& executablePath, 
    const std::vector<std::string>& tokens)
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
