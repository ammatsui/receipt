
#include "utils.hpp"
#include "gui.hpp"

#include <iostream> // for debugging



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


char* openfilename(HWND hwnd)    
{
    char* fileNameStr;
    LPSTR filebuff = new char[256];
    OPENFILENAME open = { 0 };
    open.lStructSize = sizeof(OPENFILENAME);
    open.hwndOwner = hwnd; //Handle to the parent window
    open.lpstrFilter = imageFiles;
    open.lpstrCustomFilter = NULL;
    open.lpstrFile = filebuff;
    open.lpstrFile[0] = '\0';
    open.nMaxFile = 256;
    open.nFilterIndex = 1;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = "Select An Image File\0";
    open.nMaxFileTitle = strlen("Select an image file\0");
    open.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

    if (GetOpenFileName(&open))
    {
        fileNameStr = open.lpstrFile;
    }

    return fileNameStr;
}


char* savefilename(HWND hwnd)
{
    char* fileNameStr;
    LPSTR filebuff = new char[256];
    OPENFILENAME open = { 0 };
    open.lStructSize = sizeof(OPENFILENAME);
    open.hwndOwner = hwnd; //Handle to the parent window
    open.lpstrFilter = allFiles;
    open.lpstrCustomFilter = NULL;
    open.lpstrFile = filebuff;
    open.lpstrFile[0] = '\0';
    open.nMaxFile = 256;
    open.nFilterIndex = 1;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = "Select A File\0";
    open.nMaxFileTitle = strlen("Select a file\0");
    open.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&open))
    {
        fileNameStr = open.lpstrFile;
    }

    return fileNameStr;
}


bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    #if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    #endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}


void upload_file(GLFWwindow* window)
{
    scanned = false;
                
    HWND handle = glfwGetWin32Window(window);
                
    filename_in = openfilename(handle);
            
    bool ret = LoadTextureFromFile(filename_in, &in_image_texture, &in_image_width, &in_image_height);
    IM_ASSERT(ret);

    show_file_window = true;             
    uploaded = true;
}


void save_file(GLFWwindow* window)
{
    entries.insert(entries.end(), new_entries.begin(), new_entries.end() );
    new_entries = {}; cnt = 0;

    HWND handle = glfwGetWin32Window(window);

    filename_out = savefilename(handle);

    std::fstream res;
   
    res.open(filename_out, std::fstream::out);

    /* header */
    for (int i = 0; i < headers.size()-1; i++)
    {
        res << headers[i] << sep;
    }
    res << headers[headers.size()-1] << std::endl;

    for (int i = 0; i < entries.size(); i++)
    {
        for (int j = 0; j < entries[0].size()-1; j++)
        {
            res << entries[i][j] << sep;
        }
        res << entries[i][entries[0].size()-1] << std::endl;
    }


    res.close();

}


void show_ocrSettings()
/* choose settings for ocr */
{
    ImGui::Text("Choose language of a receipt:");
    ImGui::RadioButton("ukr", &ocr_lang, 0); ImGui::SameLine();
    ImGui::RadioButton("eng", &ocr_lang, 1); 

            
    ImGui::Text("Choose OCR settings:");
    ImGui::RadioButton("full",  &type, 0); ImGui::SameLine();
    ImGui::RadioButton("lines", &type, 1); ImGui::SameLine();
    ImGui::RadioButton("words", &type, 2); 
}


void show_table()
{
    int n_cols = entries[0].size();
    int n_rows = entries.size();

    float column_width = ImGui::GetWindowWidth()/(2*headers.size());

    // note: cnt starts with 0, so rows contains only the additional rows.
    if (ImGui::Button("Add row"))
        { 
            rows[cnt] = true;
            cnt++;
        } 

    

    
    if (ImGui::BeginTable("Result", n_cols, ImGuiTableFlags_SizingFixedFit))
    {
        for (int i = 0; i < headers.size(); i++)
        {
            ImGui::TableSetupColumn(headers[i], 0, column_width);
        }
        ImGui::TableHeadersRow();

        int row = 0;
        
        /* display prefilled rows */
        while (row < n_rows)  
        {
            ImGui::TableNextRow();
            for (int column = 0; column < n_cols; column++)
            {
                ImGui::TableSetColumnIndex(column);
           
           
                std::string fld = "##" + std::to_string(row) + std::to_string(column);        
                ImGui::InputText(fld.c_str(), &entries[row][column]); 
              
            }
            row++;
        }

        while (new_entries.size() < cnt)
        {
            std::vector<std::string> empty(n_cols, "");
            new_entries.push_back(empty);  
        }
    
        for (int i = 0; i < cnt; i++)
        {
            ImGui::TableNextRow();
            for (int column = 0; column < n_cols; column++)
            {   
                ImGui::TableSetColumnIndex(column);
               
                std::string fld = "##" + std::to_string(row) + std::to_string(column);        
                ImGui::InputText(fld.c_str(), &new_entries[i][column]); //&entries[row][column]); 

            }
            row++; 
        }
    
        ImGui::EndTable();
    }
}



//int gui_start()
int main()
{
    /* setup window */
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    /* decide GL+GLSL versions */
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    /* create window */ //with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Scanner", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    /* setup Dear ImGui context */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
 

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);


    // TODO: font size adaptive to window size

    //TODO: setStyle();

    /* state */
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    

    /* main loop */
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int wdth, hght;
        glfwGetWindowSize(window, &wdth, &hght);
        ImGui::SetNextWindowSize(ImVec2(wdth, hght)); // ensures ImGui fits the GLFW window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        
        if (show_main_window)
        {    
            ImGui::Begin("Scanner", &show_main_window, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);   


            /* choose language menu */
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Language"))
                {
                    if (ImGui::MenuItem("Українська")) { /* lang = "ukr"; set_lang(); */ }
                    if (ImGui::MenuItem("English"))    { /* lang = "eng"; set_lang(); */ }
       
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            } 
            

            ImGui::Text("Choose and upload a photo of a receipt to scan:");
            /* upload - loads the file; then choose settings */
            if (ImGui::Button("Upload"))                           
            {
                entries = {};
                new_entries = {};
                upload_file(window);
            }

            if (uploaded)
            {
                show_ocrSettings();
                /* scan the image and display the scanned result together with output table */
                if (ImGui::Button("Scan"))                           
                {
                    // TODO: add loading button while scanning works
                    entries = scanning(filename_in, ocr_lang, type); // returns scanned and parsed text (stream probably)
                    // TODO: scan the image and display the result in a table with the correct settings
                    // images are opened in new windows that can be closed!
                    // save the text: save, save as, save and close, close (pop up warning - save without closing?)
                    
                    scanned = true;

                    
                }

                if (scanned)
                {
                    show_table(); 
                    if (ImGui::Button("Save"))
                    {
                        save_file(window);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Close"))
                    {
                        //TODO: popup close without saving?

                        /* return to default values */
                        uploaded = false;
                        scanned = false;
                        show_file_window = false;
                        type = 0;
                        ocr_lang = 0;
                        cnt = 0;
                        entries = {};
                        filename_in = "";
                        filename_out = "";
                
                    }
                }

            }

            if (show_file_window)
            {
                // make it BeginChild() to open window in the original window 
                // (but need to think about coordinates and fixed size)
                
               // ImGui::Begin("file", &show_file_window);//, ImGuiWindowFlags_MenuBar);
                ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowWidth() - in_image_width/2, 0));
                ImGui::BeginChild("file", ImVec2(ImGui::GetWindowWidth()/2, ImGui::GetWindowHeight()));

                //ImGui::Image((void*)(intptr_t)in_image_texture, ImVec2(in_image_width, in_image_height));
                /* maybe scale it down, if the image is too big (and it is) */
                ImGui::Image((void*)(intptr_t)in_image_texture, ImVec2(in_image_width*0.5f, in_image_height*0.5f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

                ImGui::EndChild();
                //ImGui::End();
                
            }

            // if (show_scan_window)
            // {
            //     // make it BeginChild() to open window in the original window
            //     ImGui::Begin("scan", &show_scan_window);//, ImGuiWindowFlags_MenuBar);

            //     ImGui::Image((void*)(intptr_t)out_image_texture, ImVec2(out_image_width, out_image_height));

            //     ImGui::End();
                
            // }
          


            ImGui::End();
            
        }

       

        /* rendering */
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
   
    return 0;
}

