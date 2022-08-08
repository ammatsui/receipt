#include "preproc.hpp"




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



float get_skew_angle(cv::Mat& src, int method)
{
    cv::Mat thresh;
    std::vector<std::vector<cv::Point> > cnts;
    std::vector<cv::Point> contour;
    cv::RotatedRect box;
   
    cv::imshow("src", src);
    cv::waitKey();
    src.copyTo(thresh);
    float angle = 0;
    if (src.channels() > 1)
    {
      cvtColor(thresh, thresh, cv::COLOR_BGR2GRAY);
    }
    GaussianBlur(thresh, thresh, cv::Size(9, 9), 0);
    threshold(thresh, thresh, 0, maxValue, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
    auto ker = getStructuringElement(cv::MORPH_RECT, cv::Size(30, 5));
    dilate(thresh, ker, 5);
    findContours(thresh, cnts, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    /* here are some different methods of choosing the angle - that of the largest box, the mean of all, the angle of the medium box */
    switch (method)
    {
        case 0:
            contour = cnts[0];
            box = minAreaRect(contour);
            angle = box.angle;
            break;
        case 1:
            contour = cnts[(int)(cnts.size()/2)];
            box = minAreaRect(contour);
            angle = box.angle;
            break;
        case 2:
            // TODO
            break;
      default:
            break;
    }
    if (angle < -45)
    {
        angle = 90 + angle;
    }
    return (-1.0 * angle);
}



float get_skew_angle(cv::Mat& src)
{
    return get_skew_angle(src, 0);
}



void rotate(cv::Mat& src, cv::Mat& dst, float angle)
{
    auto T = getRotationMatrix2D(cv::Point(src.cols/2, src.rows/2), angle, 1.0);
    warpAffine(src, dst, T, cv::Size(src.cols, src.rows), cv::INTER_CUBIC, cv::BORDER_REPLICATE);
}



void deskew(cv::Mat& src, cv::Mat& dst, int method)
{
    float angle = get_skew_angle(src, method);
    rotate(src, dst, angle);
}



void deskew(cv::Mat& src, cv::Mat& dst)
{
    float angle = get_skew_angle(src);
    rotate(src, dst, angle);
}



void scan(cv::Mat& image, cv::Mat& scan)
/* returns scan-like image of receipt without background */
{
    cv::Mat mask, grey;
    std::vector<std::vector<cv::Point> > cnts;
    std::vector<cv::Point2f> coords;
    cv::Point2f pnts[4];

    /* rescale (tesseract seems to like that size) */
    float wpercent = (basewidth/float(image.size[0]));
    float hsize = (float(image.size[1])*float(wpercent));
    cv::resize(image, image, cv::Size(), wpercent, wpercent);


    cvtColor(image, grey, cv::COLOR_BGR2GRAY);
    GaussianBlur(grey, mask, kernel, 0);
    erode(mask, mask, getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1)));
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
    erode(scan, scan, getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    threshold(scan, scan, 0, maxValue, cv::THRESH_BINARY + cv::THRESH_OTSU); 

    cv::imwrite("scan.png", scan);
}


// void scan(std::string& path, cv::Mat& res)
void scan(const char* path, cv::Mat& res)
{
    res = cv::imread(path); 
    scan(res, res);
}


void get_boxes(cv::Mat& src, std::vector<cv::Rect>& boxes)
{
    // TODO: add assertion that src has 1 channel */
    std::vector<std::vector<cv::Point>> cnts;
    std::vector<std::vector<cv::Point2f>> words;
    //std::vector<cv::Rect> boxes;
    cv::Mat mask;

    boxes = {};

  //  cv::resize(src, src, cv::Size(), 0.5, 0.5);
    /* size when it works */
    /* rescale (tesseract seems to like that size) */
    float wpercent = (basewidth/float(src.size[0]))/2;
    float hsize = (float(src.size[1])*float(wpercent))/2;
    cv::resize(src, src, cv::Size(), wpercent, wpercent);
  
    int horizontal_size = src.cols / 15 ;//10; //30   15 is the best now
    /* create structure element for extracting horizontal lines through morphology operations */
    cv::Mat horizontalStructure = getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_size, 1));
    /* apply morphology operations */
    cv::Mat horizontal;
    erode(src,         horizontal, horizontalStructure, cv::Point(-1, -1));
    dilate(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));

    threshold(horizontal, mask, 0, maxValue, cv::THRESH_BINARY);  //threshVal instead of 0
    /* objects should be white and background should be black */
    findContours(~mask, cnts, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
       
    /* or choose only those with width >> height */
    float maxArea =  src.cols * src.rows ;/// 5;///4;// 145;  //250   ??? maybe it is not needed
    /* could be another function boxes = clean(cnts) */
    for (auto contour : cnts)
    {
        auto rect = boundingRect(contour); 
        if (contourArea(contour) <= maxArea && rect.width > rect.height * ratio) 
        {
            // rectangle(horizontal, rect, (255, 0, 0), 2);
            // rectangle(src, rect, (255, 0, 0), 2);

            boxes.push_back(rect);
    
        }
    }
    
    // imshow("hor", horizontal);
    // imshow("src", src);
    // cv::waitKey();
}



cv::Mat crop(cv::Mat& src, cv::Rect& roi)
{
    /* create a mask for each strip to mask out that region from src */
    cv::Mat m = cv::Mat::zeros(src.size(), CV_8U);
    if ((0 <= roi.x && 0 <= roi.width && roi.x + roi.width   <= m.cols && 
         0 <= roi.y && 0 <= roi.height && roi.y + roi.height <= m.rows))
    {
            src(roi).copyTo(m); 
    }
    return m;
}



void get_lines(cv::Mat& src, std::vector<cv::Mat>& strips)
/* maybe return vector of (vector of boxes in the same line) and tell tesseract to parse it as one line */
{
    std::vector<cv::Rect> boxes;
    std::vector<cv::Point> points;
    cv::Rect roi;
    // cv::Mat line, imageROI;
    strips = {};

    get_boxes(src, boxes);
    int i = 0;
    while (i < boxes.size())
    {
       // rectangle(src, boxes[i], (255, 0, 0), 2);
        /* crop out strips */
        /* strip = boxes that are on the same level, i.e. differ by epsilon */
        cv::Rect rect = boxes[i];
        /* choose boxes on the same level (store then in tmp) */
        std::vector<cv::Rect> tmp;

       
        while ((abs(rect.y - boxes[i].y) < coef*rect.height || 
                abs(rect.y - boxes[i].y) < coef*boxes[i].height))  
        {
            tmp.push_back(boxes[i]);
            i++;
        }
        /* construct a rectangle */
        /* bounding rect of all those rects */
        flatten(tmp, points);
        roi = boundingRect(points);

       // rectangle(src, roi, (255, 0, 0), 2);

        /* create a mask for each strip to mask out that region from src */
        cv::Mat m = crop(src, roi);
        strips.push_back(m);    
    }
    // cv::imshow("src", src);
    // cv::waitKey();
}



void get_lines(cv::Mat& src, std::vector<std::vector<cv::Mat>>& strips)
/* maybe return vector of (vector of boxes in the same line) and tell tesseract to parse it as one line */
{
    std::vector<cv::Rect> boxes;
    std::vector<cv::Point> points;
    cv::Rect roi;
    strips = {};

    get_boxes(src, boxes);
    int i = 0;
    while (i < boxes.size())
    {
        /* crop out strips */
        /* strip = boxes that are on the same level, i.e. differ by epsilon */
        cv::Rect rect = boxes[i];
        /* choose boxes on the same level (store then in box_line) */
        std::vector<cv::Mat> box_line;
        
        while ((abs(rect.y - boxes[i].y) < coef*rect.height || 
                abs(rect.y - boxes[i].y) < coef*boxes[i].height))  
        {
            auto cropped = crop(src, boxes[i]);
            box_line.push_back(cropped);
            i++;
        }
        strips.push_back(box_line); 
    }

}


void add_border(cv::Mat& src, cv::Mat& dst)
{
    // int w = std::max(W, src.cols + 2*offset);
    // int h = std::max(H, src.rows + 2*offset);
    if (src.cols > 0 && src.rows > 0)
    {
    int w = src.cols + offset;
    int h = src.rows + offset;
    dst = ~cv::Mat::zeros(cv::Size(w, h), CV_8U);
    int x = (dst.cols - src.cols) / 2;
    int y = (dst.rows - src.rows) / 2;    
    src.copyTo(dst(cv::Rect(x, y, src.cols, src.rows)));
    }
    return;
}