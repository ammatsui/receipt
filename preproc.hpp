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

const float ratio = 0.8; // 1.75; for short words like `tax` it needs to be bigger

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
void flatten(std::vector<cv::Rect_<_Tp>>& rects, std::vector<cv::Point_<_Tp>>& points);

template <typename _Tp>
std::vector<cv::Point_< _Tp >> order(cv::Point_< _Tp > pnts[]);

template <typename _Tp>
std::vector<cv::Point_< _Tp >> order(std::vector<cv::Point_< _Tp >> pnts);

template <typename _Tp>
static bool y_axis(const std::vector<cv::Point_< _Tp >> box_1, const std::vector<cv::Point_< _Tp >> box_2);

template <typename _Tp>
static bool boxOrder(const cv::Rect_<_Tp> box1, const cv::Rect_<_Tp> box2);

void warpTransform(cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& pnts);

float get_skew_angle(cv::Mat& src, int method);

float get_skew_angle(cv::Mat& src);

void rotate(cv::Mat& src, cv::Mat& dst, float angle);

void deskew(cv::Mat& image, cv::Mat& scan, int method);

void deskew(cv::Mat& image, cv::Mat& scan);

void scan(cv::Mat& image, cv::Mat& scan);

// void scan(std::string& path, cv::Mat& res); // {res = cv::imread(path); scan(res, res);}
void scan(const char* path, cv::Mat& res);

void get_boxes(cv::Mat& src, std::vector<cv::Rect>& boxes);

cv::Mat crop(cv::Mat& src, cv::Rect& roi);

void get_lines(cv::Mat& src, std::vector<cv::Mat>& strips);

void get_lines(cv::Mat& src, std::vector<std::vector<cv::Mat>>& strips);

void add_border(cv::Mat& src, cv::Mat& dst);

// void preproc(std::string path);



template <typename _Tp>
void flatten(std::vector<cv::Rect_<_Tp>>& rects, std::vector<cv::Point_<_Tp>>& points)
{
   points = {};
   cv::Point_<_Tp> vrts[4];
   for (int i = 0; i < rects.size(); i++)
   {
    vrts[0].x = rects[i].x;
    vrts[0].y = rects[i].y;
    points.push_back(vrts[0]);

    vrts[1].x = rects[i].x + rects[i].width;
    vrts[1].y = rects[i].y;
    points.push_back(vrts[1]);

    vrts[2].x = rects[i].x + rects[i].width;
    vrts[2].y = rects[i].y + rects[i].height;
    points.push_back(vrts[2]);

    vrts[3].x = rects[i].x;
    vrts[3].y = rects[i].y + rects[i].height;
    points.push_back(vrts[3]);     
   } 
}



template <typename _Tp>
std::vector<cv::Point_< _Tp >> order(cv::Point_< _Tp > pnts[])
{
    /* points with smallest y coords - tops, largest - bottoms */
    /* out of them, choose left and right in the correct order */
    std::vector<cv::Point_< _Tp >> tmp;
    for (int i = 0; i < 4; i++)
    {
        tmp.push_back(pnts[i]);
    }
    /* sort by y coord values */
    sort(tmp.begin(), tmp.end(), compareY<_Tp>);
    /* tops */
    std::vector<cv::Point_< _Tp >> tm(tmp.begin(), tmp.begin()+2);
    /* bottoms */
    std::vector<cv::Point_< _Tp >> bm(tmp.end()-2, tmp.end());

    /* sort by x coord values */
    sort(tm.begin(), tm.end(), compareX<_Tp>);
    sort(bm.begin(), bm.end(), compareX<_Tp>);

    /* store in the correct order - tl, tr, br, bl */
    tmp[0] = tm[0];
    tmp[1] = tm[1];
    tmp[2] = bm[1];
    tmp[3] = bm[0];

    return tmp;
}


// TODO: this is the same as the previous one, use template for container
template <typename _Tp>
std::vector<cv::Point_< _Tp >> order(std::vector<cv::Point_< _Tp >> pnts)
{
    /* points with smallest y coords - tops, largest - bottoms */
    /* out of them, choose left and right in the correct order */
    std::vector<cv::Point_< _Tp >> tmp;
    for (int i = 0; i < 4; i++)
    {
        tmp.push_back(pnts[i]);
    }
    /* sort by y coord values */
    sort(tmp.begin(), tmp.end(), compareY<_Tp>);
    /* tops */
    std::vector<cv::Point_< _Tp >> tm(tmp.begin(), tmp.begin()+2);
    /* bottoms */
    std::vector<cv::Point_< _Tp >> bm(tmp.end()-2, tmp.end());

    /* sort by x coord values */
    sort(tm.begin(), tm.end(), compareX<_Tp>);
    sort(bm.begin(), bm.end(), compareX<_Tp>);

    /* store in the correct order - tl, tr, br, bl */
    tmp[0] = tm[0];
    tmp[1] = tm[1];
    tmp[2] = bm[1];
    tmp[3] = bm[0];

    return tmp;
}



template <typename _Tp>
static bool y_axis(const std::vector<cv::Point_< _Tp >> box_1, const std::vector<cv::Point_< _Tp >> box_2)
{
   auto box1 = order(box_1);
   auto box2 = order(box_2);

   if (compareY(box1[0], box2[0]) && compareY(box1[3], box2[3]))
   {
        return true;
   }
   if (compareY(box1[0], box2[0]) && ~compareY(box1[3], box2[3]))
   {
        return compareX(box1[0], box2[0]);
   }
   if (~compareY(box1[0], box2[0]) && ~compareY(box1[3], box2[3]))
   {
        return false;
   }
   if (~compareY(box1[0], box2[0]) && compareY(box1[3], box2[3]))
   {
        return compareX(box1[0], box2[0]);
   }
   return false;

}


template <typename _Tp>
static bool boxOrder(const cv::Rect_<_Tp> box1, const cv::Rect_<_Tp> box2)
{
   if (compareY(cv::Point_<_Tp>(box1.x, box1.y), cv::Point_<_Tp>(box2.x, box2.y)))
   {
        return true;
   }
   else 
   {
        return (compareX(cv::Point_<_Tp>(box1.x, box1.y), cv::Point_<_Tp>(box2.x, box2.y)));
   }
}

#endif
