#include <fstream>

#include "error.h"
#include "config.h"

Config::Config(const std::string &fname)
{
    this->filename_ = fname;
    ReadConf();
}

bool Config::KeyExists(const std::string &key) const
{
    return (contents_.find(key) != contents_.end());
}

void Config::RemoveComment(std::string &line) const
{
    /*
     * find the string and remove part of sequence which leading by ';',
     * if no matches found, std::string.find() will return npos value.
     *
     * */
    if(line.find(';') != line.npos) {
        line.erase(line.find(';'));
    }
}

bool Config::OnlyWhiteSpace(const std::string &line) const
{
    return (line.find_first_not_of(' ') == line.npos);
}

bool Config::IsValidLine(const std::string &line) const
{
    std::string temp = line;
    temp.erase(0, temp.find_first_not_of("\t "));  // remove all whitespaces in front of the line

    if(temp[0] == '=')
    {
        return false;
    }

    for(size_t i = temp.find('='); i < temp.length(); i++)
    {
        if(temp[i] != ' ')
            return true;
    }

    return false;
}

void Config::ExtractKey(std::string &key, size_t const &sepPos,
                        const std::string &line) const
{
    key = line.substr(0, sepPos);
    if(key.find('\t') != line.npos || key.find(' ') != line.npos)
    {
        key.erase(key.find_first_of("\t "));  // remove trailing whitespace
    }
}

void Config::ExtractValue(std::string &value, size_t const &sepPos,
                          const std::string &line) const {
    value = line.substr(sepPos + 1);
    value.erase(0, value.find_first_not_of("\t "));  // remove leading whitespace
    value.erase(value.find_last_not_of("\t ") + 1);  // remove trailing whitespace
}

void Config::ExtractContents(const std::string &line)
{
    std::string temp = line;
    temp.erase(0, temp.find_first_not_of("\t "));  // remove leading whitespace of the line
    size_t sepPos = temp.find('=');

    std::string key;
    std::string value;
    ExtractKey(key, sepPos, temp);
    ExtractValue(value, sepPos, temp);

    if(!KeyExists(key))
    {
        contents_.insert(std::pair<std::string, std::string>(key, value));
    }
    else
    {
        ExitError("Can only have unique key!\n");
    }
}

void Config::ParseLine(const std::string &line, size_t const line_no)
{
    if(line.find('=') == line.npos)
    {
        ExitError("Couldn't find separator on line: " + Convert::T_to_str<int>(line_no) + "\n");
    }

    if(!IsValidLine(line))
    {
        ExitError("Wrong format for line: " + Convert::T_to_str<int>(line_no) + "\n");
    }

    ExtractContents(line);
}

void Config::ReadConf()
{
    std::ifstream file;
    file.open(filename_.c_str());
    if(!file)
    {
        ExitError("Couldn't read file: " + filename_ + "\n");
    }

    std::string line;
    size_t line_no = 0;
    while(std::getline(file, line))
    {
        line_no++;
        std::string temp = line;
        //std::cout << line << std::endl;

        if(temp.empty()) continue;

        RemoveComment(temp);
        if(OnlyWhiteSpace(temp)) continue;

        ParseLine(temp, line_no);
    }

    file.close();
}

