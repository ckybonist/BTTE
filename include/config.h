#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

#include <iostream>

#include <string>

#include <map> // holding the pair of key-value

#include "Convert.h"

class Config {
    public:
      Config(const std::string&);
      bool KeyExists(const std::string&) const;
      template <typename ValueType>
      ValueType GetValueOfKey(const std::string &key,
                ValueType const &defaultType = ValueType()) const {
            if(!KeyExists(key)) {
                return defaultType;
            }
        return Convert::str_to_T<ValueType>(contents.find(key)->second);
      }

    private:
        std::map<std::string, std::string> contents;
        std::string fname;
        void RemoveComment(std::string&) const;
        bool OnlyWhiteSpace(const std::string&) const;
        bool IsValidLine(const std::string&) const;
        void ExtractKey(std::string &, size_t const&, const std::string&) const;
        void ExtractValue(std::string&, size_t const&, const std::string&) const;
        void ExtractContents(const std::string&);
        void ParseLine(const std::string&, size_t const);
        void ReadConf();
};

#endif // for #ifndef _CONFIGFILE_H
