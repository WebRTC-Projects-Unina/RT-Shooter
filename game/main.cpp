#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>

#include <GL/gl.h>
#include <GLES3/gl3.h>


#include <GLFW/glfw3.h> 
#include <stdbool.h>

#include "main.h"
#include "res/shaders/BlinnPhong.shader"
#include "res/shaders/Phong.shader"
#include "res/shaders/Flat.shader"
#include "res/shaders/Gouraud.shader"

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
     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Dopo aver inizializzato GLFW e creato una finestra:
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Inizializza i backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);  // 'window' è il tuo GLFWwindow*
    ImGui_ImplOpenGL3_Init("#version 300 es");      // Adatta allo shader version che usi

   
    //stampo versione di OpenGL installata
    std::cout << "Versione OpenGL: " << glGetString(GL_VERSION) << std::endl;

    //attiva test depth sui fragment
    glEnable(GL_DEPTH_TEST);
    
  
    //flatShader = CreateShader(FlatShaderVertex, FlatShaderFragment);

    //gouraudShader = CreateShader(GouraudShaderVertex, GouraudShaderFragment);
    
    //phongShader = CreateShader(PhongShaderVertex, PhongShaderFragment);

	blinnPhongShader = CreateShader(BlinnPhongShaderVertex, BlinnPhongShaderFragment);

    shader = blinnPhongShader;
    glUseProgram(shader);



    mode = 1;
    old_model = 0;
    current_model = 0;;
    /* 
     * Quando chiudo la finestra viene alzato il flag "glfwWindowShouldClose"
     * e lo devo controllare perchè devo poi effettivamente devo "terminare"
     * la finestra con "glfwDestroyWindow(window)" ma in questo caso uso direttamente
     * "glfwTerminate()" poichè chiude tutte le finestre aprte e arresta glfw
     */

        

     emscripten_set_main_loop(rendering_loop, 0, 1);

    //glDeleteProgram(phongShader);
    //glDeleteProgram(gouraudShader);
    //glDeleteProgram(flatShader);
    glDeleteProgram(blinnPhongShader);


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
