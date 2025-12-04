#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>

#include <GL/gl.h>
#include <GLES3/gl3.h>


#include <GLFW/glfw3.h> 
#include <stdbool.h>

#include "main.h"
#include "res/shaders/BlinnPhong.shader"
#include "res/shaders/SkyBox.shader"


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <fstream>
#include <string>
#include <sstream>

#include "renderer/renderer.h"





EMSCRIPTEN_KEEPALIVE
int main(void)
{   


    /* Initialize the library */
    if (!glfwInit())
        return -1;

    oldFrame = glfwGetTime();
    

    //aggiungo l'MSAA (Multisample Antialiasing) (senza il triangolo è a scalette)
    glfwWindowHint(GLFW_SAMPLES, 32);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(winWidth, winHeight, "Window", NULL, NULL);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    // Dopo aver inizializzato GLFW e creato una finestra:
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.Fonts->AddFontFromFileTTF("./res/fonts/Inter_28pt-Bold.ttf", 24.f);
    ImGui::StyleColorsDark();
    // Inizializza i backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);  
    //ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
    ImGui_ImplOpenGL3_Init("#version 300 es");      

   

    //attiva test depth sui fragment
    glEnable(GL_DEPTH_TEST);
    
  
	blinnPhongShader = CreateShader(BlinnPhongShaderVertex, BlinnPhongShaderFragment);
    skyBoxShader = CreateShader(SkyBoxShaderVertex, SkyBoxShaderFragment);
    shader = blinnPhongShader;
    glUseProgram(shader);


 
    mode = 1;


     emscripten_set_main_loop(rendering_loop, 0, 1);

    glDeleteProgram(blinnPhongShader);
    glDeleteProgram(skyBoxShader);



    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
