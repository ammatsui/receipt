#include "parser.hpp"


template <typename O1, typename O2>
void parse(O1& text, O2& parsed)
{
    std::string line, tmp, item;
    std::smatch m, v;
    std::string price;
    while (std::getline(text, line))
    {
        std::regex_search(line, m, priceRE);
        if (m.size() > 0)
        {
            tmp = m[0];
            std::regex_search(tmp, v, valueRE);
            price = v[0];
            item = std::regex_replace(line, priceRE, "");
            parsed << item << sep << price << std::endl;
            std::cout << item << " " << price << std::endl;
            if (item == "CHANGE" || item == "change")
            {
                break;
            }
        }
    }
}



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



template <typename O1, typename O2>
void process(O1& text, O2& res)
{
    res << header << std::endl;
    parse(text, res);
}
