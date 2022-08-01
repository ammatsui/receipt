#ifndef OCR_H   
#define OCR_H

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


#include "preproc.hpp"

template <typename O>
void ocr(cv::Mat& image, O& out, int method);
/* method - full text, line separations, etc */

/* default method is full text */
template <typename O>
void ocr(cv::Mat& image, O& out) {ocr(image, out, 0);}

template <typename O>
void _ocr_full(cv::Mat& image, O& out);

template <typename O>
void _ocr_lines(cv::Mat& image, O& out);


template <typename O>
void ocr(cv::Mat& image, O& out, int method)
{
    // scan(image, image);
    switch (method)
    {
        case 0:
            _ocr_full(image, out);
            break;
        case 1:
            _ocr_lines(image, out);
            break;
        default:
            break;
    }
}



template <typename O>
void _ocr_full(cv::Mat& image, O& out)  // image is already a scan
{  
    std::string outText;  
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

    ocr->SetPageSegMode(tesseract::PSM_SINGLE_COLUMN);

    ocr->SetImage(image.data, image.cols, image.rows, 1, image.step);

    outText = std::string(ocr->GetUTF8Text());

    out << outText;

    ocr->End();
}



template <typename O>
void _ocr_lines(cv::Mat& image, O& out)
{
    std::vector<cv::Mat> lines;
    cv::Mat line, im;
    std::string outText;
    get_lines(image, lines);
    
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

    ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  
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

#endif
