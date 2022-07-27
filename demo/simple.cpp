#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>

#include "preproc.hpp"

using namespace std;
using namespace cv;

int main()
{
  string outText;

  Mat im = cv::imread("image.png");
  scan(im, im);
    
  tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
  ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

  ocr->SetPageSegMode(tesseract::PSM_SINGLE_COLUMN);

  ocr->SetImage(im.data, im.cols, im.rows, 1, im.step);

  outText = string(ocr->GetUTF8Text());

  cout << outText;

  Boxa* bounds = ocr -> GetWords(NULL);
  for (int i = 0; i < bounds->n; ++i) 
  {
    Box* b = bounds->box[i];
    cv::rectangle(im, { b->x,b->y,b->w,b->h }, { 0, 255, 0 }, 2);
  }

  imshow("image", im);
  waitKey();

  ocr->End();

  return 0;
}
