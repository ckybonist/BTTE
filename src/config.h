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
    ValueType GetValueOfKey(const std::string& key,
                            ValueType const& defaultType = ValueType()
                           ) const
    {
        if (!KeyExists(key))
        {
            return defaultType;
        }
        return Convert::str_to_T<ValueType>(contents_.find(key)->second);
    }

  private:
    void RemoveComment(std::string& line) const;
    bool OnlyWhiteSpace(const std::string& line) const;
    bool IsValidLine(const std::string& line) const;
    void ExtractKey(std::string& key, size_t const& sepPos, const std::string& line) const;
    void ExtractValue(std::string& value, size_t const& sepPos, const std::string& line) const;
    void ExtractContents(const std::string& line);
    void ParseLine(const std::string& line, size_t const line_no);
    void ReadConf();

    std::map<std::string, std::string> contents_;
    std::string filename_;
};

#endif // for #ifndef _CONFIG_H
