#include "preproc.hpp"

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



/* takes the image and rectangle and returns a scan */
void warpTransform(cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& pnts)
{
    /* calculate the height and width of the scan, which is the max dims from the original */
    float widthA = sqrt((pow(pnts[2].x - pnts[3].x, 2) + pow(pnts[2].y - pnts[3].y, 2)));
	  float widthB = sqrt((pow(pnts[1].x - pnts[0].x, 2) + pow(pnts[1].y - pnts[0].y, 2)));
	  float max_width = std::max(int(widthA), int(widthB));

	  float heightA = sqrt((pow(pnts[2].y - pnts[1].y, 2) + pow(pnts[2].x - pnts[1].x, 2)));
	  float heightB = sqrt((pow(pnts[3].y - pnts[0].y, 2) + pow(pnts[3].x - pnts[0].x, 2)));
	  float max_height = std::max(int(heightA), int(heightB));
    
    /* the resulting points */
    std::vector<cv::Point2f> res { {0, 0}, {max_width-1, 0}, 
    {max_width-1, max_height-1}, {0, max_height-1} };
    
    /* get the matrix transformation */
    cv::Mat matr = getPerspectiveTransform(pnts, res);
    warpPerspective(src, dst, matr, cv::Size(max_width, max_height));
}



void scan(cv::Mat& image, cv::Mat& scan)
/* threshold value, turns the receipt into white blob, everything else goes black */
{
    cv::Mat mask, grey;
    std::vector<std::vector<cv::Point> > cnts;
    std::vector<cv::Point2f> coords;
    cv::Point2f pnts[4];

    /* rescale (for tesseract) */
    float wpercent = (basewidth/float(image.size[0]));
    float hsize = (float(image.size[1])*float(wpercent));
    cv::resize(image, image, cv::Size(), wpercent, wpercent);

    cvtColor(image, grey, cv::COLOR_BGR2GRAY);
    GaussianBlur(grey, mask, kernel, 0);
    threshold(mask, mask, threshVal, maxValue, cv::THRESH_BINARY); 

    findContours(mask, cnts, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    
    float maxArea = 0.0;
    float area;
    for (auto contour : cnts)
    {
        auto tmp = minAreaRect(contour); 
        area = contourArea(contour);

        if (area >= maxArea)
        {
            tmp.points(pnts);
            maxArea = area;
        }
    }

    /* order the points and put them into a vector */
    coords = order(pnts);
    warpTransform(grey, scan, coords);
    /* save the result */
    cv::imwrite("scan.png", scan);
}


void get_boxes(cv::Mat& src, std::vector<cv::Rect>& boxes)
{
    // TODO: add assertion that src has 1 channel */
    std::vector<std::vector<cv::Point>> cnts;
    std::vector<std::vector<cv::Point2f>> words;
    cv::Mat mask;

    boxes = {};

    cv::resize(src, src, cv::Size(), 0.5, 0.5);
  
    int horizontal_size = src.cols / 10;//10;  //30
    /* create structure element for extracting horizontal lines through morphology operations */
    cv::Mat horizontalStructure = getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_size, 1));
    /* apply morphology operations */
    cv::Mat horizontal;
    erode(src,         horizontal, horizontalStructure, cv::Point(-1, -1));
    dilate(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));

    threshold(~horizontal, mask, threshVal, maxValue, cv::THRESH_BINARY);
  
    findContours(mask, cnts, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
       
    /* choose only those with width >> height */
    float maxArea =  src.cols * src.rows ;///4;// 145;  //250   ??? maybe it is not needed
    for (auto contour : cnts)
    {
        auto rect = boundingRect(contour); 
        if (contourArea(contour) <= maxArea && rect.width > rect.height * ratio)
        {
            boxes.push_back(rect);
        }
    }
}



void get_lines(cv::Mat& src, std::vector<cv::Mat>& strips)
{
    std::vector<cv::Rect> boxes;
    std::vector<cv::Point> points;
    cv::Rect roi;
    
    strips = {};

    get_boxes(src, boxes);
    int i = 0;
    while (i < boxes.size())
    {
        /* crop out the strips, i.e. boxes (words) that are on the same level */
        cv::Rect rect = boxes[i];
        /* store then in tmp */
        std::vector<cv::Rect> tmp;
        while ((abs(rect.y - boxes[i].y) < coef*rect.height      || 
                abs(rect.y - boxes[i].y) < coef*boxes[i].height) || 
                i == boxes.size())  
        {
            tmp.push_back(boxes[i]);
            i++;
        }
        /* construct a shared  rectangle = bounding rect of all those rects */
        flatten(tmp, points);
        roi = boundingRect(points);

        cv::Mat line, imageROI;

        /* create a mask for each strip to mask out that region from src */
        cv::Mat m = cv::Mat::zeros(src.size(), CV_8U);
        if ((0 <= roi.x && 0 <= roi.width && roi.x + roi.width <= m.cols && 0 <= roi.y && 0 <= roi.height && roi.y + roi.height <= m.rows))
        {
            src(roi).copyTo(m); 
            strips.push_back(m);
         }
    }
}



void add_border(cv::Mat& src, cv::Mat& dst)
{
    int w = std::max(W, src.cols + offset);
    int h = std::max(H, src.rows + offset);
    dst = ~cv::Mat::zeros(cv::Size(w, h), CV_8U);
    int x = (dst.cols - src.cols) / 2;
    int y = (dst.rows - src.rows) / 2;
    src.copyTo(dst(cv::Rect(x, y, src.cols, src.rows)));
}
