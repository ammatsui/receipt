# receipt
Receipt scanner + parser

## contains
`preproc` files do the preprocessing steps such that crop the image of a receipt and turn it into a scan-like image.
Before and after:
<img src="https://user-images.githubusercontent.com/54817160/182263635-aa9971f5-56ec-4389-ab66-daa7783a7776.png" width="300"> <img src="https://user-images.githubusercontent.com/54817160/182263638-ab8dd140-65e7-4d5c-a636-d0bc7c27b7f9.png" width="300">


`ocr` does the text extraction.
There are several methods: 
just pass the `scan` result to `ocr` and let it to its job; 
pass lines with text line by line and extract text from each of them separately; 
pass lines (which are just a set of words) and extract text word by word.

<img src = "https://user-images.githubusercontent.com/54817160/182264500-dec119fa-da5c-423f-818d-e3816307ba3f.png" width=100> <img src = "https://user-images.githubusercontent.com/54817160/182264501-797ec955-3db1-4998-9eaf-aac611b076f1.png" width=100> <img src = "https://user-images.githubusercontent.com/54817160/182264502-ab26cba8-9d3b-4486-8320-7ddb1d60c13e.png" width=100>

 `parser` saves the extracted data into a `.csv` file in the format `Item,Price`.

Future improvements are to be made.

## dependencies
opencv
tesseract
leptonica
