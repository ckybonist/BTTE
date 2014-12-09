#include <cstdlib>

#include "error.h"

void ExitError(const std::string& err_msg)
{
    std::cout << "Error Occurred: " << err_msg << std::endl;
    exit(EXIT_FAILURE);
}
