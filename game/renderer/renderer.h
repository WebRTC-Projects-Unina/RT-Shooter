#ifndef RENDERER_H
#define RENDERER_H




    #include <iostream>
    #include <fstream>
    #include <string>
    #include <sstream>
    #include <stdbool.h>

    #include <GL/gl.h>
    #include <GLES3/gl3.h>
    #include <GLFW/glfw3.h> 


    #include <glm.hpp>
    #include <gtc/matrix_transform.hpp>
    #include <gtc/type_ptr.hpp>

    #include "imgui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"


    #define WINDOW_WIDTH_INIT 1280  
    #define WINDOW_HEIGHT_INIT 720
    #define CAMERA_SPEED 4.0f


    inline int winWidth = WINDOW_WIDTH_INIT, winHeight = WINDOW_HEIGHT_INIT;

    inline std::vector<int> map;
    inline int map_height, map_width;
    inline GLFWwindow* window; 
    inline unsigned int blinnPhongShader;
    inline unsigned int skyBoxShader;

    inline unsigned int shader; 
    inline int mode; 

    inline float* vertices; 
    inline float dim; 
    inline double oldFrame;
    inline unsigned int buffer;

    inline float Ka = 1.0f, Kd = 1.0f, Ks = 5.0f ,shininess = 80.0f;
    inline glm::vec3 lightPos(3.0f, 2.0f, 1.0f);
    inline glm::vec3 ambientColor(1.0f, 0.4118f, 0.0f);      // #FF6900
    inline glm::vec3 diffuseColor(1.0f, 0.4118f, 0.0f);      // #FF6900
    inline glm::vec3 specularColor(1.0f, 1.0f, 1.0f);        // #FFFFFF
    inline glm::vec3 backgroundColor(34.0f/255.0f, 34.0f/255.0f, 34.0f/255.0f); // rgba(34, 34, 34, 1)
 
    inline bool diffuseEnable = true, ambientEnable = true, specularEnable = true;






    inline glm::vec3 enemyPosition = glm::vec3(3.0f, 1.0f, 3.0f);



    inline double oldTime = glfwGetTime();
    inline double titleUpdateCooldownTime = glfwGetTime();

    unsigned int CompileShader(unsigned int type, const std::string source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    void rendering_loop();


    void load_map(const std::string& path);
    
    inline glm::vec2 translations[10000];
    inline glm::vec2 playerTranslations[2];
    inline int translations_index = 0;

#endif