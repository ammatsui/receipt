#include "utils.hpp"



void set_lang()
{
    // TODO
    return;
}

std::vector<std::vector<std::string>> scanning(const char* path, int ocr_lang, int type)
// process returns vector of pairs that is convenient to pass to table and back
{
    std::vector<std::vector<std::string>> out;
    cv::Mat im;
    std::stringstream text;
    scan(path, im);
    ocr(im, text, type, ocr_lang); 
    process(text, out); // maybe it also needs to know ocr_lang
    return out;
}