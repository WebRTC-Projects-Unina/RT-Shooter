

    
#include <iostream>

#include <GL/gl.h>
#include <GLES3/gl3.h>


#include <GLFW/glfw3.h> 
#include <stdbool.h>

#include "../main.h"
#include "renderer.h"



#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fstream>
#include <string>
#include <sstream>

#include "../dependencies/json/json.hpp"


using json = nlohmann::json;


void load_map(const std::string& path){
    std::ifstream file(path);
    json j;
    file >> j;


    // Prende l'array "data" come std::vector<int>
    map = j["layers"][0]["data"].get<std::vector<int>>();
    map_height = j["layers"][0]["height"].get<int>();
    map_width = j["layers"][0]["width"].get<int>();

    for (int i = 0; i < map_width; i++) {
        for (int j = 0; j < map_width; j++) {
            if (map[i+(j*map_width)] == 1){
                //i-0.5, j-1.5
                glm::vec2 translation;
                translation.x = ((float)i)+0.5;
                translation.y = ((float)j)+0.5;
                translations[translations_index++] = translation;
            }
            else if (map[i+(j*map_width)] == 2 && spawnPointNumber < 30) {
                spawnPoints[spawnPointNumber] = glm::vec3((float)i, .0f, (float)j);
                spawnPointNumber++;
            }
        }
    }

}