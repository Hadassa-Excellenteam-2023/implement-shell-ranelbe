#include "Pipes.h"

/*
* create pipes and store their fds in m_pipefds
* @param pipeCount - the number of pipes to create.
*/
Pipes::Pipes(int pipeCount) : m_pipefds(2 * pipeCount) 
{
    for (int i = 0; i < pipeCount; ++i) {
        if (pipe(&m_pipefds[2 * i]) < 0) {
            throw std::runtime_error(ERROR_CREATE_PIPE);
        }
    }
}

/*
* get the input fds of the pipe at the given index.
* @param index - the index of the pipe.
*/
int Pipes::getInputPipeFD(int index) const
{
    return m_pipefds[2 * index];
}

/*
* get the output fds of the pipe at the given index.
* @param index - the index of the pipe.
*/
int Pipes::getOutputPipeFD(int index) const
{
    return m_pipefds[2 * index + 1];
}

/*
* close all the pipes.
*/
void Pipes::closePipes()
{
    for (int fd : m_pipefds) {
        close(fd);
    }
}

