
#include <iostream>

#include <GL/gl.h>
#include <GLES3/gl3.h>

#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <GLFW/glfw3.h> 
#include <stdbool.h>
#include <iostream>
#include "../main.h"
#include "renderer.h"
#include "Mesh.h"
#include "Camera.h"

#include "../game/Player.h"

#define GLM_ENABLE_EXPERIMENTAL
#define EMSCRIPTEN_USE_EMBEDDED_GLFW3


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fstream>
#include <string>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../dependencies/stb_image/stb_image.h"


void textureLoad(unsigned int* texture, const char* path);
void mouse_callback(GLFWwindow* window);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


ClientPlayer player(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
glm::vec3 enemyPosition = glm::vec3(3.0f, 1.0f, 3.0f);

// Questa funzione verrà chiamata direttamente da JS
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnMessageFromJS(const char* msg) {
    std::string json(msg);
    std::cout << "JSON ricevuto: " << json << std::endl;

    glm::vec3 v;

    // Parsing (rozzo ma funziona per il demo)
    sscanf(json.c_str(),
           "{ \"x\": %f , \"y\": %f , \"z\": %f }",
           &v.x, &v.y, &v.z);

    enemyPosition = v;
}
}

EM_JS(void, RegisterSocketIOCallback, (), {
    if (!Module.socket) {
        console.log("❌ Socket non pronta!");
        return;
    }

    Module.socket.on("enemy_update", (pos) => {
        const json = JSON.stringify(pos);

        Module.ccall(
            "OnMessageFromJS", // nome funzione C++
            null,              // ritorno void
            ["string"],        // tipo argomento
            [json]           
        );
    });

    console.log("🔵 RegisterSocketIOCallback installato.");
});



EM_JS(void, sendPosizione, (const char* json), {
    // jsSocket deve essere la tua WebSocket aperta in JavaScript
    Module.socket.emit("player_update", JSON.parse(UTF8ToString(json)));
});


void sendVec3(glm::vec3 v) {


    std::stringstream ss;
    ss << "{ \"x\": " << v.x << ", \"y\": " << v.y << ", \"z\": " << v.z << " }";
    std::string json = ss.str();

    sendPosizione(json.c_str());
}




float lastX = winWidth / 2.0f;
float lastY = winHeight / 2.0f;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void imgui_menu_rendering() {
            //Inizia il frame ImGui
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        window_flags |= ImGuiWindowFlags_NoMove;
        //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        //ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        //ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //Disegna la GUI con ImGui
        ImGui::Begin("Parameters menu");
            
        
        float th = 0.2f;
        const ImU32 col = ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
        //draw_list->AddLine(ImVec2(center.x, center.y-25), ImVec2(center.x, center.y+5), col, th);                          
        //draw_list->AddLine(ImVec2(center.x-5, center.y), ImVec2(center.x+5, center.y), col, th);
        
        
        ImGui::SliderFloat("Ka", &Ka, 0.0f, 1.0f);
            ImGui::SliderFloat("Kd", &Kd, 0.0f, 1.0f);
            ImGui::SliderFloat("Ks", &Ks, 1.0f, 7.0f);
            ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f);
            ImGui::SliderFloat3("Light Pos", (float*)&lightPos, -20.0f, 20.0f);
            glm::vec3 playerPos = player.getPosition();
            ImGui::InputFloat3("Posizione", (float*)&playerPos);
            ImGui::InputFloat3("Enemy Pos", (float*)&enemyPosition);

        // ImGui::InputFloat("Gravità", (float*)&gravity, 0.1f, 1.0f, "%.3f");
            

            ImGui::Checkbox("Diffuse", &diffuseEnable);
            ImGui::Checkbox("Ambient", &ambientEnable);
            ImGui::Checkbox("Specular", &specularEnable);
        
            ImGui::ColorEdit3("Background Color", (float*)&backgroundColor);

        
        ImGui::End();


        
        //Rendering ImGui
            ImGui::Render();  // Chiude il frame ImGui e prepara i dati di disegno
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
}

Mesh* scimmia = nullptr;

unsigned int wall_diffTexture, wall_specTexture;
unsigned int floor_diffTexture, floor_specTexture;

 glm::vec2 playerTranslations[2];
unsigned int VBO, cubeVAO;
unsigned int instanceVBO;

unsigned int floorVBO; 
unsigned int fVBO, floorVAO;

unsigned int playerVBO; 
unsigned int pVBO, playerVAO;
        
static int last_cursor_disabled = -1;
void rendering_loop() {
    


       
        double nowFrame = glfwGetTime();
        deltaTime = nowFrame - oldFrame;
        oldFrame = nowFrame;
        


        // input
        // -----
        processInput(window);


        mouse_callback(window);
        player.updatePosition(deltaTime);


        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F)){
            emscripten_request_pointerlock("#canvas", true);
            emscripten_request_fullscreen("#canvas", true);

        }
      

 
        /* 
         * qui mi prendo la dimensione della finestra ogni frame
         * così che se cambia la dimesione aggiorna anche la dimesioe
         * della viewport (e quindi del contenuto della finestra)
         */
        glfwGetWindowSize(window, &winWidth, &winHeight),
        glViewport(0, 0, winWidth, winHeight);
        

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);

        
        
        
/*
        if (current_model != old_model) {
            objl::Loader Loader;

        bool loadout = Loader.LoadFile("./res/models/blender_monkey.obj");

        std::cout << std::endl << std::endl << Loader.LoadedMeshes[0].MeshName << std::endl;
            old_model = current_model;
            vertices = models[current_model];
            dim = dimArray[current_model];

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, dim * sizeof(float), vertices, GL_STATIC_DRAW);

        }
*/      

        if(!b_setupDone){
            RegisterSocketIOCallback();
             if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSetScrollCallback(window, scroll_callback);


            //scimmia = new Mesh("./res/models/cube.obj");
            load_map("./res/maps/test.json");


            textureLoad(&wall_diffTexture, "./res/textures/stone_brick_wall/stone_brick_wall_diff.jpg");
            textureLoad(&wall_specTexture, "./res/textures/stone_brick_wall/stone_brick_wall_specular.jpg");
            textureLoad(&floor_diffTexture, "./res/textures/floor/floor_diff.jpg");
            textureLoad(&floor_specTexture, "./res/textures/floor/floor_specular.jpg");

            //texture

             //AAAAAAA
        // first, configure the cube's VAO (and VBO)
 
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 10000, &translations[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);   

      
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);	
        glVertexAttribDivisor(3, 1);  




        glGenBuffers(1, &floorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 10000, &translations[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &fVBO);

        glBindBuffer(GL_ARRAY_BUFFER, fVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertex), floor_vertex, GL_STATIC_DRAW);

        glBindVertexArray(floorVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);   

      
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);	
        glVertexAttribDivisor(3, 1);  
            playerTranslations[0] = glm::vec2(0.f, 0.f);
            playerTranslations[1] = glm::vec2(0.f, 0.f);



        glGenBuffers(1, &playerVBO);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2, &playerTranslations[0], GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        glGenVertexArrays(1, &playerVAO);
        glGenBuffers(1, &pVBO);

        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(playerMesh), playerMesh, GL_STATIC_DRAW);

        glBindVertexArray(playerVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);   

      
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);	
        glVertexAttribDivisor(3, 1); 

            
            b_setupDone = true;

            
        }

        //scimmia->Draw();
        //3d



       
        playerTranslations[0] = glm::vec2(0.f,0.f);
        playerTranslations[1] = glm::vec2(0.f, 0.f);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2, &playerTranslations[0], GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

       




        glm::mat4 model = glm::mat4(1.f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(glm::cos((float)glfwGetTime() * glm::radians(20.0f))*0.5f, glm::sin((float)glfwGetTime() * glm::radians(20.0f)) * 1.0f, 1.0f));
        //model = glm::translate(view, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z));
        view = player.camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(45.0f), (float)winWidth/(float)winHeight, 0.001f, 100.0f);
        //projection = glm::translate(view, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z));
        unsigned int modelLoc = glGetUniformLocation(shader, "model");
        unsigned int viewLoc = glGetUniformLocation(shader, "view");

        
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_diffTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wall_specTexture);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

        glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shader, "ambientColor"),  1, glm::value_ptr(ambientColor));
        glUniform3fv(glGetUniformLocation(shader, "diffuseColor"), 1, glm::value_ptr(diffuseColor));
        glUniform3fv(glGetUniformLocation(shader, "specularColor"), 1, glm::value_ptr(specularColor));
        glUniform3f(glGetUniformLocation(shader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        glUniform1f(glGetUniformLocation(shader, "ambientEnable"), 1.0f);
        glUniform1f(glGetUniformLocation(shader, "diffuseEnable"), 1.0f);
        glUniform1f(glGetUniformLocation(shader, "specularEnable"), 1.0f);

        glUniform1f(glGetUniformLocation(shader, "Ka"), Ka);
        glUniform1f(glGetUniformLocation(shader, "Ks"), Ks);
        glUniform1f(glGetUniformLocation(shader, "Kd"), Kd);
        glUniform1f(glGetUniformLocation(shader, "shininessVal"), shininess);

        if (!ambientEnable) 
            glUniform1f(glGetUniformLocation(shader, "ambientEnable"), 0.0f);
        else
            glUniform1f(glGetUniformLocation(shader, "ambientEnable"), 1.0f);

        if (!diffuseEnable)
            glUniform1f(glGetUniformLocation(shader, "diffuseEnable"), 0.0f);
        else 
            glUniform1f(glGetUniformLocation(shader, "diffuseEnable"), 1.0f);
       
        if (!specularEnable)
            glUniform1f(glGetUniformLocation(shader, "specularEnable"), 0.0f);
        else
            glUniform1f(glGetUniformLocation(shader, "specularEnable"), 1.0f);
      
        shader = blinnPhongShader;
        glUseProgram(shader);
		

        //6 non indica il numero di vertici, ma il numero di indici
       // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); //la draw call
        /* ma non gli abbiamo passato il buffer! Invece si! Lo abbiamo fatto prima con 
        * glBindBuffer -> opengl è tipo una state machine in cui prima gli do tutto
        * e poi gli idco di fare la draw call e con quella funzione gli ho detto di usare
        * quel buffer
        */


            // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, translations_index); 

         // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floor_diffTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floor_specTexture);

        glBindVertexArray(floorVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1); 

        

        model = glm::translate(glm::mat4(1.0f), enemyPosition); 

        

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glBindVertexArray(playerVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, (sizeof(playerMesh)/sizeof(float))/8, 1); 


        imgui_menu_rendering();


        //glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.f, -5.0f));
        //model2 = glm::rotate(model2, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        //glDrawArrays(GL_TRIANGLES, 0, dim / 6);

        /* Swap front and back buffers */
     glfwSwapBuffers(window);
   int cursor_disabled = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

    if (cursor_disabled != last_cursor_disabled) { // Only log changes as to not spam console
        last_cursor_disabled = cursor_disabled;

        std::cout << std::endl << "GLFW_CURSOR_DISABLED?" << cursor_disabled << std::endl;
    }
        /* Poll for and process events */
       glfwPollEvents();
        
    emscripten_async_call([](void*) { sendVec3(player.getPosition()); }, nullptr, 0);

    
}






void textureLoad(unsigned int* texture, const char* path)
{
            glGenTextures(1, texture);
            glBindTexture(GL_TEXTURE_2D, *texture);
            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // load and generate the texture
            int width, height, nrChannels;
            unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
}



void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.processMovement(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.processMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.processMovement(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.processMovement(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.processMovement(JUMP, deltaTime);
}



// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window)
{   

    double xpos, ypos;
       //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    player.processMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    player.camera.ProcessMouseScroll(static_cast<float>(yoffset));
}