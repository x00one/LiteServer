#include <fstream>
#include <iostream>

#include "inc/Tools.h"
#include "inc/IniParser.h"

IniParser::IniParser(const char* iniPath)
{
    std::ifstream iniFile(iniPath);
    size_t pos;
    std::string paramName;
    std::string paramValue;
    
    for(std::string line; getline(iniFile, line); )
    {
        pos = line.find("=");
        
        paramName = line.substr(0, pos);
        paramValue = line.substr(pos + 1);
        
        trim(paramName);
        trim(paramValue);
        
        this->data[paramName] = paramValue;
    }
    
    /*
    std::cout << "Map content :\n";
    for (std::map<std::string, std::string>::iterator it = this->data.begin(); it != this->data.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    
    std::cout << "Map end\n";
    */
}

std::string IniParser::get(std::string paramName)
{
    std::map<std::string, std::string>::iterator it = this->data.find(paramName);
    
    if (it == this->data.end()) {
        return std::string();
    } else {
        return it->second;
    }
}