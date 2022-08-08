// #ifndef UTILS_H   
// #define UTILS_H

#pragma once
 

#include "preproc.hpp"
#include "ocr.hpp"
#include "parser.hpp"

#include <sstream>

//#include <iostream>


// const char* Name_eng = "Receipt Scanner";
// const char* Name_ukr = "Сканувальник";

// const char* ukr = "ukr";
// const char* eng = "eng";
// char* lang = (char*)ukr;

// const char* mLang_eng = "Language";
// const char* mLang_ukr = "Мова";

// const char* bUpload_eng = "Upload";
// const char* bUpload_ukr = "Завантажити";

// const char* tUpload_eng = "Choose and upload a photo of a receipt to scan:";
// const char* tUpload_ukr = "Оберіть та завантажте файл для сканування:";

// const char* rLang_eng = "Choose the language of the receipt:";
// const char* rlang_ukr = "Оберіть мову файлу:";

// const char* u_eng = "ukr";
// const char* u_ukr = "укр";

// const char* e_eng = "eng";
// const char* e_ukr = "англ";

// const char* ocr_eng = "Choose OCR settings:";
// const char* ocr_ukr = "Оберіть налаштування OCR:";

// const char* ocr_set_f_eng = "full";
// const char* ocr_set_f_ukr = "повний текст";

// const char* ocr_set_l_eng = "lines";
// const char* ocr_set_l_ukr = "лінії";

// const char* ocr_set_w_eng = "words";
// const char* ocr_set_w_ukr = "слова";


/* hardcode define and declare translations here.
also declare general ocr_set_w and so on
define them in set_lang() */


void set_lang(); 
/*
all text is stored in global variables like bUpload_eng, bUpload_ukr (those are consts)
also there are global variables like bUpload, this function assgins to them bUpload_[lang]
gui_start() uses bUpload instead of "Upload" everywhere
*/
std::vector<std::vector<std::string>> scanning(const char* path, int ocr_lang, int type); // think of a better name


//endif