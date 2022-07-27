#ifndef PREPROC_H   
#define PREPROC_H

/* 
preprocessing of the document

Convert the image to grayscale
Find the edges in the image
Use the edges to find all the contours
Select only the contours of the document
Apply warp perspective to get the top-down view of the document
Threshold the document to make it look like a 'scan'
Find ROIs - words in the document
Find ROIs - lines of text in the document
*/

#include <opencv2/opencv.hpp>

const float basewidth = 1900;

const int threshVal = 155;
const double maxValue = 255;

const float epsilon = 0.01;
const float coef = 0.75;

const float ratio = 1.75;

/* for Gaussian blur */
const int k = 5;
const cv::Size kernel = cv::Size(k, k);

/* padding the image for tessaract */
/* the values are taken from here - https://github.com/tesseract-ocr/tesseract/issues/427#issuecomment-1138703215 */
const int W = 250;
const int H = 51;

const int offset = 5;




template <typename _Tp>
static bool compareX(const cv::Point_< _Tp > p1, const cv::Point_< _Tp > p2) {return (p1.x < p2.x);}

template <typename _Tp>
static bool compareY(const cv::Point_< _Tp > p1, const cv::Point_< _Tp > p2) {return (p1.y < p2.y);}

template <typename _Tp>
static bool y_axis(const std::vector<cv::Point_< _Tp >> box_1, const std::vector<cv::Point_< _Tp >> box_2);

template <typename _Tp>
void flatten(std::vector<cv::Rect_<_Tp>>& rects, std::vector<cv::Point_<_Tp>>& points);

template <typename _Tp>
std::vector<cv::Point_< _Tp >> order(cv::Point_< _Tp > pnts[]);

template <typename _Tp>
std::vector<cv::Point_< _Tp >> order(std::vector<cv::Point_< _Tp >> pnts);

template <typename _Tp>
static bool boxOrder(const cv::Rect_<_Tp> box1, const cv::Rect_<_Tp> box2);

void warpTransform(cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& pnts);

void scan(cv::Mat& image, cv::Mat& scan);

std::vector<cv::Rect> get_boxes(cv::Mat& src);

void get_lines(cv::Mat& src, std::vector<cv::Mat>& strips);

void add_border(cv::Mat& src, cv::Mat& dst);

#endif
