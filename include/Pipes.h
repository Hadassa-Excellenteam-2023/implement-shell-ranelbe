#include <iostream>
#include <unistd.h>
#include <vector>
#include <array>
#include <stdexcept>

constexpr auto ERROR_CREATE_PIPE = "Failed to create pipe.";

class Pipes {
public:
    Pipes(int pipeCount);
    int getInputPipeFD(int index) const;
    int getOutputPipeFD(int index) const;
    void closePipes();

private:
    std::vector<int> m_pipefds;
};