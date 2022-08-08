#ifndef OCR_H   
#define OCR_H

#pragma once
 
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <regex>
#include <sstream>

#include "preproc.hpp"


static const char* whiteList = " \n,.*#$:;-=\"\'0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


// template <typename O>
// void ocr(cv::Mat& image, O& out, int method, char* lang);
// /* method - full text, line separations, etc */

// /* default method is full text */
// template <typename O>
// void ocr(cv::Mat& image, O& out, int lang) {ocr(image, out, 0, lang);}

// template <typename O>
// void _ocr_full(cv::Mat& image, O& out, int lang);

// template <typename O>
// void _ocr_lines(cv::Mat& image, O& out, int lang);

// template <typename O>
// void _ocr_wordlines(cv::Mat& image, O& out, int lang);

inline const char* _convert(int lang)
{
    switch (lang)
    {
        case 0:
            return (char*)"ukr";
            break;
        case 1:
            return (char*)"eng";
            break;
        default:
            return (char*)"";
            break;
    }
}



template <typename O>
void _ocr_full(cv::Mat& image, O& out, int lang)
{
    std::string outText;  
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    auto language = _convert(lang);
    ocr->Init(NULL, language, tesseract::OEM_LSTM_ONLY);

    ocr->SetPageSegMode(tesseract::PSM_SINGLE_COLUMN);

    ocr->SetImage(image.data, image.cols, image.rows, 1, image.step);

    outText = std::string(ocr->GetUTF8Text());

    out << outText;

    ocr->End();
}



template <typename O>
void _ocr_lines(cv::Mat& image, O& out, int lang)
{
    std::vector<cv::Mat> lines;
    cv::Mat line, im;
    std::string outText;
    get_lines(image, lines);
    
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    auto language = _convert(lang);
    ocr->Init(NULL, language, tesseract::OEM_LSTM_ONLY);

    ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

    ocr->SetVariable("tessedit_char_whitelist", whiteList);
  
    for (int i = lines.size()-1; i >= 0; i--)
    {
        /* add border for tesseract */
        add_border(lines[i], line);
        dilate(line, line, getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)));

        // cv::imshow("line", line);
        // cv::waitKey();
  
        ocr->SetImage(line.data, line.cols, line.rows, 1, line.step);

        outText = std::string(ocr->GetUTF8Text());

        out << outText;
    
    }

    ocr->End();
}



template <typename O>
void _ocr_wordlines(cv::Mat& image, O& out, int lang)
{
    std::vector<std::vector<cv::Mat>> lines;
    cv::Mat line, im;
    std::string outText;
    get_lines(image, lines);
    
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    auto language = _convert(lang);
    ocr->Init(NULL, language, tesseract::OEM_LSTM_ONLY);

    ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

    ocr->SetVariable("tessedit_char_whitelist", whiteList);
  
    for (int i = lines.size()-1; i >= 0; i--)
    {
        std::string tmp;
        for (int j = 0; j < lines[i].size(); j++)
        {
            /* add border for tesseract */
            add_border(lines[i][j], line);
            dilate(line, line, getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)));

            // cv::imshow("line", line);
            // cv::waitKey();
  
            ocr->SetImage(line.data, line.cols, line.rows, 1, line.step);

            outText = std::string(ocr->GetUTF8Text());

            tmp += outText;

            /* remove newline characters */
            tmp = std::regex_replace(tmp, std::regex("\\r\\n|\\r|\\n"), " ");

        }
        out << tmp << "\n";
       // std::cout << tmp << std::endl;
    
    }

    ocr->End();
}


template <typename O>
void ocr(cv::Mat& image, O& out, int method, int lang)
{
    // scan(image, image);
    switch (method)
    {
        case 0:
            _ocr_full(image, out, lang);
            break;
        case 1:
            _ocr_lines(image, out, lang);
            break;
        case 2:
            _ocr_wordlines(image, out, lang);
        default:
            break;
    }
}


/* default method is full text */
template <typename O>
void ocr(cv::Mat& image, O& out, int lang) {ocr(image, out, 0, lang);}


#endif