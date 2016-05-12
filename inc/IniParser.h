#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <map>
#include <string>

class IniParser
{
protected:
    std::map<std::string, std::string> data;
    
public:
    IniParser(const char* iniPath);
    std::string get(std::string paramName);
};

#endif /* INI_PARSER_H */
