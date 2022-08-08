#ifndef GUI_H   
#define GUI_H


#pragma once


#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <windows.h>
#pragma comment(lib, "comdlg32.lib")


#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h> 
#include <GLFW/glfw3native.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


/* image upload */
#define STB_IMAGE_IMPLEMENTATION
#define GL_CLAMP_TO_EDGE 0x812F
#include "stb_image.h"



static bool show_main_window = true;
static bool show_file_window = false;
// static bool show_scan_window = false;
// static bool show_text_window = false;

static bool uploaded = false;
static bool scanned  = false;

const char* filename_in;
const char* filename_out;  // either derived from filename_in or user-defined

/* display images */
int in_image_width;
int in_image_height;
GLuint in_image_texture;

int out_image_width;
int out_image_height;
GLuint out_image_texture;

static int ocr_lang = 0;
static int type = 0;

static int cnt = 0;
static std::vector<bool> rows(256, false);

static std::vector<const char*> headers{"Item", "Price"};
static std::vector<std::vector<std::string>> entries;
static std::vector<std::vector<std::string>> new_entries;

static const char* allFiles = (char*)"All Files (*.*)\0*.*\0";
static const char* imageFiles = "Image Files\0*.jpg;*.png;*.bmp;*.jpeg\0\0"; //"Image Files(.jpg|.png|.bmp|.jpeg)\0*.jpg;*.png;*.bmp;*.jpeg\0\0";

static const int wOffset = 100;

void left(GLFWwindow* window);

void right(GLFWwindow* window);

static void glfw_error_callback(int error, const char* description);

char* openfilename(HWND hwnd = NULL);
// maybe make those two the same function, passing filter and flags
char* savefilename(HWND hwnd = NULL);

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

void upload_file(GLFWwindow* window);

void save_file(GLFWwindow* window);

void show_ocrSettings();

void show_table();

int gui_start();

#endif