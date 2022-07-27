#include "preproc.hpp"
#include "ocr.hpp"
#include "parser.hpp"

using namespace std;
using namespace cv;


void get_data(string path)
{
    stringstream text;
    fstream res;
    /* the result will be saved to file with the same name, but .csv */
    string name = get_name(path);
    Mat im = imread(path);
    res.open(name + ".csv", fstream::out);
    /* text contains the ocr's output */
    ocr(im, text); 
    /* saves the output into a .csv file: item,price */
    process(text, res);  
    res.close();
}

int main()
{
    /* path to the image file */
    string path = "image.png";
    get_data(path);

    return 0;
}
