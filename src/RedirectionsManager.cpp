#include "RedirectionsManager.h"
#include <iostream>


/* 
* handle redirections - search for redirection symbols 
* in the command and redirect the IO accordingly.
* @param tokens - the command and its arguments.
*/
void RedirectionsManager::handleRedirections(std::vector<std::string>& tokens)
{
    for (auto it = tokens.begin()+1 ; it != tokens.end(); ++it) {
        auto redirectionSymbol = _redirectionSymbols.find(*it);

        if (redirectionSymbol != _redirectionSymbols.end()) {
            // A redirection symbol was found
            const std::string& symbol = redirectionSymbol->first;
            int stdFd = redirectionSymbol->second.first;
            int flags = redirectionSymbol->second.second;

            if (it + 1 != tokens.end()) {
                reset();
                const std::string& filename = *(it + 1);
                redirectIO(stdFd, filename, flags);
                // remove the symbol and the filename from the tokens vector
                it = tokens.erase(it, it + 2);
                --it;
            } else {
                throw std::runtime_error(ERROR_MISSING_FILENAME);
            }
        }
    }
}


/*
* redirect the IO to the given file.
* @param stdFd - the std file descriptor to redirect.
* @param filename - the file to redirect to.
* @param flags - the flags for the open system call.
*/
void RedirectionsManager::redirectIO(int stdFd, const std::string& filename, int flags)
{
    // Store the original file descriptor before redirection
    int originalFd = dup(stdFd);
    if (originalFd == -1) {
        throw std::runtime_error(ERROR_STORE_FD);
    }

    // Open the file
    int fd = open(filename.c_str(), flags, 0666);
    if (fd == -1) {
        throw std::runtime_error(ERROR_OPEN_FILE);
    }

    // Redirect the file descriptor
    if (dup2(fd, stdFd) == -1) {
        close(fd);
        throw std::runtime_error(ERROR_REDIRECT_FD);
    } 
    if (close(fd) == -1) {
		throw std::runtime_error(ERROR_CLOSE_FD);
	}

    // store the original file descriptor
    m_originalFd = std::make_pair(originalFd, stdFd);
}


/*
* reset the IO to the original file descriptor.
*/
void RedirectionsManager::reset()
{
    //check if the pair is not empty
    if (m_originalFd) {
        if (dup2(m_originalFd->first, m_originalFd->second) == -1) {
            throw std::runtime_error(ERROR_RESTORE_FD);
        }
        if (close(m_originalFd->first) == -1) {
            throw std::runtime_error(ERROR_CLOSE_FD);
        }
	}
    m_originalFd.reset();
}