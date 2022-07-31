#include "parser.hpp"


std::string get_name(std::string path)
{
    size_t lastindex = path.find_last_of("."); 
    std::string name = path.substr(0, lastindex); 
    return name;
}



void process(std::string path)
{
    std::fstream res;
    std::fstream text(path);
    auto name = get_name(path);
    res.open(name+".csv", std::fstream::out);
    res << header << std::endl;
    parse(text, res);
    res.close();
    text.close();
}
