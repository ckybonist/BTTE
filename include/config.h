#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

#include <iostream>
//#include <string>
#include <map> // holding the pair of key-value

#include "convert.h"

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
        return Convert::str_to_T<ValueType>(contents_.find(key)->second);
      }

    private:
        std::map<std::string, std::string> contents_;
        std::string fname_;
        void RemoveComment_(std::string&) const;
        bool OnlyWhiteSpace_(const std::string&) const;
        bool IsValidLine_(const std::string&) const;
        void ExtractKey_(std::string &, size_t const&, const std::string&) const;
        void ExtractValue_(std::string&, size_t const&, const std::string&) const;
        void ExtractContents_(const std::string&);
        void ParseLine_(const std::string&, size_t const);
        void ReadConf_();
};

#endif // for #ifndef _CONFIGFILE_H
