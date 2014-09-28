#ifndef _CONFIG_H
#define _CONFIG_H

#include <iostream>
#include <map> // holding the pair of key-value
//#include <string>

#include "convert.h"

class Config
{
    public:
      Config(const std::string& filename);
      bool KeyExists(const std::string& key) const;

      template <typename ValueType>
      ValueType GetValueOfKey(const std::string &key,
                              ValueType const &defaultType = ValueType()
                             ) const
      {
            if(!KeyExists(key))
            {
                return defaultType;
            }
        return Convert::str_to_T<ValueType>(_contents.find(key)->second);
      }

    private:
        void _RemoveComment(std::string& line) const;
        bool _OnlyWhiteSpace(const std::string& line) const;
        bool _IsValidLine(const std::string& line) const;
        void _ExtractKey(std::string& key, size_t const& sepPos, const std::string& line) const;
        void _ExtractValue(std::string& value, size_t const& sepPos, const std::string& line) const;
        void _ExtractContents(const std::string& line);
        void _ParseLine(const std::string& line, size_t const line_no);
        void _ReadConf();

        std::map<std::string, std::string> _contents;
        std::string _filename;
};

#endif // for #ifndef _CONFIG_H
