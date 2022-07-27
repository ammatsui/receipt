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

#endif
