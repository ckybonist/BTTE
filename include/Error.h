#ifndef _ERROR_H
#define _ERROR_H

#include <iostream>
#include <cstdlib>

inline void exitWithError(const std::string &error)
{
    std::cout << error << std::endl;
    exit(EXIT_FAILURE);
}

#endif // for #ifndef _ERROR_H
