// #include "src/libs/preproc.hpp"
// #include "src/libs/ocr.hpp"
// #include "src/libs/parser.hpp"
#include "preproc.hpp"
#include "ocr.hpp"
#include "parser.hpp"

using namespace std;
using namespace cv;

/*
pre-recognition modyfications:

Convert the image to grayscale
Find the edges in the image
Use the edges to find all the contours
Select only the contours of the document
Apply warp perspective to get the top-down view of the document

Korobacz et al. (2019) [48] used a similar approach to the one in [46]. The
image was first converted to grayscale and then binarized. After this, Hough
Line Transform was applied to skew correct the image. The position of the text
was then extracted by utilizing the Canny Edge Detection method (see Section
2.5.3.4). From these contours, the bounding box was calculated and the image
was cropped thereafter. Experiments were run on a small data set, consisting of
a maximum of 50 receipt images. Using the OCR engine ABBYY Finereader,
the results showed that the proposed approach increased the text recognition
efficiency by 25% on average.

Fletcher and Kasturi

add some threshold somewhere to get more contrast in the result to make recognition easier

after that, read the text

store the text in the file

later add GUI (Dear ImGUI)
*/



/* 
preproc.hpp - scan the receipt
.hpp - extract text via tesseract
parser.hpp
gutils.hpp - GUI interface : upload image (open file), get back file with result (download it), also just show the output text

show the parsed text in a table (via GUI), allow user to change values in the table 
(to correct tessaract's mistakes)

also add filenames (when file is being saved user can either give it a name, 
or the program would give it something like scan001, 
but need to think of how to implement consistent filenames here
NO: the program names it imageFileName.txt (or whatever))
*/

void get_data(string path)
{
    stringstream text;
    fstream res;
    string name = get_name(path);
    // Mat im = imread(path);
    // scan(im, im);
    Mat im;
    scan(path, im);
    res.open(name + ".csv", fstream::out);
    ocr(im, text, 0); 
    process(text, res);  
    res.close();
}

int main()
{
    string path = "image.png";
    get_data(path);

    return 0;
}