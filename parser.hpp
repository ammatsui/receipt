#ifndef PARSE_H   
#define PARSE_H



#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

/* the price pattern at the end of the line */
const std::regex priceRE("\\ [0-9]+\\.[0-9]+\\ ?[A-Z]?$");
const std::regex valueRE("[0-9]+\\.[0-9]+");

const char sep = ',';
const std::string header = "Item,Price";



template <typename O1, typename O2>
void parse(O1& text, O2& parsed);

std::string get_name(std::string path);

/* parse data from file in the path, creates output file and saves data there */
void process(std::string path);

/* parse data from stream */
template <typename O1, typename O2>
void process(O1& text, O2& res);




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



template <typename O1, typename O2>
void process(O1& text, O2& res)
{
    res << header << std::endl;
    parse(text, res);
}

#endif
