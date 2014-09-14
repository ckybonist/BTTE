#include <cstdlib>

#include "error.h"

void ExitError(const std::string &err_msg) {
    std::cout << err_msg << std::endl;
    exit(EXIT_FAILURE);
}
