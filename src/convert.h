#ifndef _CONVERT_H
#define _CONVERT_H

#include <iostream>
// #include <string>
#include <sstream>
#include <typeinfo>

#include "error.h"

class Convert
{
public:
    template <typename T>
    static std::string T_to_str(T const& val)
    {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    template <typename T>
    static T str_to_T(std::string const& val)
    {
        std::istringstream iss(val);
        T result;
        if (!(iss >> result))
        {
            ExitError("Not a valid " +
                      (std::string)typeid(T).name() +
                      " received!\n");
        }
        return result;
    }
};

#endif // for #ifndef _CONVERT_H
