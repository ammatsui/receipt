#include "parser.hpp"


std::string get_header(char* lang)
{
    std::string header;
    if (lang == "ukr")
    {
        header = header_ukr;
    }
    else if (lang == "eng")
    {
        header = header_eng;
    }
    return header;
}


std::string get_name(const std::string path)
{
    size_t lastindex = path.find_last_of("."); 
    std::string name = path.substr(0, lastindex); 
    return name;
}



void process(const char* path, char* lang)
{
    std::fstream res;
    std::fstream text(path);
    auto name = get_name(path);
    res.open(name+".csv", std::fstream::out);
    auto header = get_header(lang);
    res << header << std::endl;
    parse(text, res);
    res.close();
    text.close();
}
