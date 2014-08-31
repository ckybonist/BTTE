#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

#include <iostream>

#include <string>

#include <map> // holding the pair of key-value

#include "Convert.h"

class Config
{
public:
    Config(const std::string&);
    bool keyExists(const std::string&) const;
    template <typename ValueType>
    ValueType getValueOfKey(const std::string &key, ValueType const &defaultType = ValueType()) const
    {
        if(!keyExists(key))
        {
            return defaultType;
        }
        return Convert::str_to_T<ValueType>(contents.find(key)->second);
    }

private:
    std::map<std::string, std::string> contents;
    std::string fName;
    void removeComment(std::string&) const;
    bool onlyWhiteSpace(const std::string&) const;
    bool isValidLine(const std::string&) const;
    void extractKey(std::string &, size_t const&, const std::string&) const;
    void extractValue(std::string&, size_t const&, const std::string&) const;
    void extractContents(const std::string&);
    void parseLine(const std::string&, size_t const);
    void readConf();
};

#endif // for #ifndef _CONFIGFILE_H
