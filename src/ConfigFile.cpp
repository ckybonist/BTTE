#include <fstream>

#include "Error.h"
#include "ConfigFile.h"

ConfigFile::ConfigFile(const std::string &fName)
{
    this->fName = fName;
    readConf();
}

bool ConfigFile::keyExists(const std::string &key) const
{
    return (contents.find(key) != contents.end());
}


void ConfigFile::removeComment(std::string &line) const
{
    /* find the string and remove part of sequence which leading by ';',
     * if no matches found, std::string.find() will return npos value.
     * */
    if(line.find(';') != line.npos)
    {
        line.erase(line.find(';'));
    }
}

bool ConfigFile::onlyWhiteSpace(const std::string &line) const
{
    return (line.find_first_not_of(' ') == line.npos);
}

bool ConfigFile::isValidLine(const std::string &line) const
{
    std::string temp = line;
    temp.erase(0, temp.find_first_not_of("\t "));  // remove all whitespaces in front of the line

    if(temp[0] == '=')
        return false;

    for(size_t i = temp.find('='); i < temp.length(); i++)
    {
        if(temp[i] != ' ')
            return true;
    }

    return false;
}

void ConfigFile::extractKey(std::string &key, size_t const &sepPos, const std::string &line) const
{
    key = line.substr(0, sepPos);
    if(key.find('\t') != line.npos || key.find(' ') != line.npos)
    {
        key.erase(key.find_first_of("\t "));  // remove trailing whitespace
    }
}

void ConfigFile::extractValue(std::string &value, size_t const &sepPos, const std::string &line) const
{
    value = line.substr(sepPos + 1);
    value.erase(0, value.find_first_not_of("\t "));  // remove leading whitespace
    value.erase(value.find_last_not_of("\t ") + 1);  // remove trailing whitespace
}

void ConfigFile::extractContents(const std::string &line)
{
    std::string temp = line;
    temp.erase(0, temp.find_first_not_of("\t "));  // remove leading whitespace of the line
    size_t sepPos = temp.find('=');

    std::string key;
    std::string value;
    extractKey(key, sepPos, temp);
    extractValue(value, sepPos, temp);

    if(!keyExists(key))
    {
        contents.insert(std::pair<std::string, std::string>(key, value));
    }
    else
    {
        exitWithError("Can only have unique key!\n");
    }
}

void ConfigFile::parseLine(const std::string &line, size_t const lineNo)
{
    if(line.find('=') == line.npos)
        exitWithError("Couldn't find separator on line: " + Convert::T_to_str<int>(lineNo) + "\n");

    if(!isValidLine(line))
        exitWithError("Wrong format for line: " + Convert::T_to_str<int>(lineNo) + "\n");

    extractContents(line);
}

void ConfigFile::readConf()
{
    std::ifstream file;
    file.open(fName.c_str());
    if(!file)
    {
        exitWithError("Couldn't read file: " + fName + "\n");
    }

    std::string line;
    size_t lineNo = 0;
    while(std::getline(file, line))
    {
        lineNo++;
        std::string temp = line;
        //std::cout << line << std::endl;

        if(temp.empty())
            continue;

        removeComment(temp);
        if(onlyWhiteSpace(temp))
            continue;

        parseLine(temp, lineNo);
    }

    file.close();
}

