#ifndef PARSE_H   
#define PARSE_H



#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

/* the price pattern at the end of the line */
const std::regex priceRE("\\ [0-9]+\\.? ?[0-9]+\\ ?[A-Z]?$");
const std::regex valueRE("[0-9]+\\.? ?[0-9]+");

// const std::string sep = ",";
// const std::string header = "Item" + sep + "Price";
const std::string sep = ",";
const std::string header_eng = "Item"    + sep + "Price";
const std::string header_ukr = "Продукт" + sep + "Ціна";


template <typename O>
void parse(O& text, std::vector<std::vector<std::string>>& out);

template <typename O1, typename O2>
void parse(O1& text, O2& parsed);

std::string get_name(const std::string path);

/* parse data from file in the path, creates output file and saves data there */
void process(const char* path);

/* parse data from stream */
template <typename O1, typename O2>
void process(O1& text, O2& res, char* lang);

template <typename O>
void process(O& text, std::vector<std::vector<std::string>>& out);

char* get_header(const char* lang);




template <typename O>
void parse(O& text, std::vector<std::vector<std::string>>& out)
{
    std::string line, tmp, item;
    std::smatch m, v;
    std::string price;

    out = {};

    while (std::getline(text, line))
    {
        std::regex_search(line, m, priceRE);
        if (m.size() > 0)
        {
            std::vector<std::string> entry;
            tmp = m[0];
            std::regex_search(tmp, v, valueRE);
            price = v[0];
            item = std::regex_replace(line, priceRE, "");

            entry.push_back(item);
            entry.push_back(price);

            out.push_back(entry);
            
            if (item == "CHANGE" || item == "change" || item == "РЕШТА" || item == "решта")
            {
                break;
            }
           
        }
    }
}


template <typename O1, typename O2>
void parse(O1& text, O2& parsed)
{
    std::string line, tmp, item, price;
    std::smatch m, v;

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
            if (item == "CHANGE" || item == "change" || item == "РЕШТА" || item == "решта")
            {
                break;
            }
       
        }
    }
}



template <typename O1, typename O2>
void process(O1& text, O2& res, char* lang)
{
    auto header = get_header(lang);
    res << header << std::endl;
    parse(text, res);
}


template <typename O>
void process(O& text, std::vector<std::vector<std::string>>& out)
{
    parse(text, out);
}

#endif
