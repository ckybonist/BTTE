/* Convert between std::string and primitive types, and vice versa. */
//#include <sstream>

/* Get type name */
//#include <typeinfo>

//#include "Convert.h"
//#include "Error.h"

/*
template <typename T>
std::string Convert::T_to_str(T const &val)
{
    std::ostringstream oss; oss << val; return oss.str();
}

template <typename T>
T Convert::str_to_T(std::string const &val)
{
    std::istringstream iss(val);
    T result;
    if(!(iss >> result))
    {
        exitWithError("Not a valid " + (std::string)typeid(T).name() + " received!\n ");
    }
    return result;
}

template <>
inline std::string Convert::str_to_T(std::string const &val)
{
    return val;
}
*/

