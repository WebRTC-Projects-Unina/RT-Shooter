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

#define GLM_ENABLE_EXPERIMENTAL

#include "menu.h"

#include "../game/Player.h"



#define EMSCRIPTEN_USE_EMBEDDED_GLFW3

#include "emjs_functions.h"

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
void shoot_raycast();


int b_lastPressed_KEY_APOSTROPHE = 0;
int b_lastPressed_KEY_M = 0;
int b_lastPressed_KEY_T = 0;
int b_lastPressed_MOUSE_LEFT = 0;

// Variabili per il sistema di sparo
float lastShootTime = 0.0f;
float shootCooldown = 0.2f;  // 200ms tra uno sparo e l'altro (5 spari/sec)

// Variabili per respawn
float deathTime = 0.0f;
float respawnDelay = 5.0f;  // 5 secondi prima del respawn

// Questa funzione verrà chiamata direttamente da JS




float lastX = winWidth / 2.0f;
float lastY = winHeight / 2.0f;
bool firstMouse = true;

float frameDeltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float lastPosUpdateDeltaTime = 0.0f;
float lastPosUpdate = 0.0f;




unsigned int wall_diffTexture, wall_specTexture;
unsigned int floor_diffTexture, floor_specTexture;
unsigned int enemy_diffTexture, skyBox_Texture;


unsigned int VBO, cubeVAO;
unsigned int instanceVBO;

unsigned int floorVBO; 
unsigned int fVBO, floorVAO;

unsigned int playerVBO; 
unsigned int pVBO, playerVAO;


unsigned int skyBoxVBO; 
unsigned int sVBO, skyBoxVAO;
        
static int last_cursor_disabled = -1;




void rendering_loop() {
    
   
        double nowFrame = glfwGetTime();
        frameDeltaTime = nowFrame - oldFrame;
        oldFrame = nowFrame;
        
    
        winWidth =  canvas_get_width();
   
        winHeight = canvas_get_height();
   
glfwSetWindowSize(window, winWidth, winHeight);

        //mando pacchetti 64 tick/sec (Valve assumimi -> what you see is what you get)
        lastPosUpdateDeltaTime = nowFrame - lastPosUpdate;
        if(lastPosUpdateDeltaTime > 0.015625) {
            emscripten_async_call([](void*) { sendVec3(player.getPosition()); }, nullptr, 0);
            lastPosUpdate = nowFrame;
        }


        // input
        processInput(window);
        if(b_pause_menu_rendering == false) mouse_callback(window);
        player.updatePosition(frameDeltaTime);


 
        /* 
         * qui mi prendo la dimensione della finestra ogni frame
         * così che se cambia la dimesione aggiorna anche la dimesioe
         * della viewport (e quindi del contenuto della finestra)
         */
        //glfwGetWindowSize(window, &winWidth, &winHeight); //non funziona bene con emscripten 
        glViewport(0, 0, winWidth, winHeight);
        

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);

        
        
        if(!b_setupDone){
            
            RegisterSocketIOCallback(); //impsota callback per ricezione socket

            if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSetScrollCallback(window, scroll_callback);

            
            load_map("./res/maps/test.json");


            //texture
            textureLoad(&wall_diffTexture, "./res/textures/stone_brick_wall/stone_brick_wall_diff.jpg");
            textureLoad(&wall_specTexture, "./res/textures/stone_brick_wall/stone_brick_wall_specular.jpg");
            textureLoad(&floor_diffTexture, "./res/textures/floor/floor_diff.jpg");
            textureLoad(&floor_specTexture, "./res/textures/floor/floor_specular.jpg");
            textureLoad(&enemy_diffTexture, "./res/textures/enemy/enemy_diff.jpg");
            textureLoad(&skyBox_Texture, "./res/textures/skybox/skybox.jpg");




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




            
            glGenBuffers(1, &skyBoxVBO);
            glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 10000, &translations[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0); 

            glGenVertexArrays(1, &skyBoxVAO);
            glGenBuffers(1, &sVBO);

            glBindBuffer(GL_ARRAY_BUFFER, sVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyBokMesh), skyBokMesh, GL_STATIC_DRAW);

            glBindVertexArray(skyBoxVAO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);   


            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);	
            glVertexAttribDivisor(3, 1);  



            imgui_style_setup();

            b_setupDone = true;

            
        }

        //3d

       
 
       

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

        
   

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

        glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shader, "ambientColor"),  1, glm::value_ptr(ambientColor));
        glUniform3fv(glGetUniformLocation(shader, "diffuseColor"), 1, glm::value_ptr(diffuseColor));
        glUniform3fv(glGetUniformLocation(shader, "specularColor"), 1, glm::value_ptr(specularColor));
        glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(player.camera.Position));

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
      

		

        //6 non indica il numero di vertici, ma il numero di indici
       // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); //la draw call
        /* ma non gli abbiamo passato il buffer! Invece si! Lo abbiamo fatto prima con 
        * glBindBuffer -> opengl è tipo una state machine in cui prima gli do tutto
        * e poi gli idco di fare la draw call e con quella funzione gli ho detto di usare
        * quel buffer
        */

        //skybox render
        /*
        glDepthMask(GL_FALSE);
        glUseProgram(blinnPhongShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyBox_Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wall_specTexture);
        model = glm::translate(glm::mat4(1.0f), player.getPosition()); 
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1); 
        glDepthMask(GL_TRUE);
        */
      
        glUseProgram(blinnPhongShader);
        

            // render the cube
                 // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_diffTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wall_specTexture);
        

       

        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, translations_index); 

    

         // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floor_diffTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floor_specTexture);
        glBindVertexArray(floorVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1); 
        
        
        setPlayerDistance(glm::distance(player.getPosition(), enemyPlayer.getPosition()));
        model = glm::translate(glm::mat4(1.0f), glm::vec3(enemyPlayer.getPosition().x, enemyPlayer.getPosition().y - 0.5,enemyPlayer.getPosition().z)); 

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, enemy_diffTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, enemy_diffTexture);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(enemyPlayer.getPosition().x, enemyPlayer.getPosition().y - 0.5,enemyPlayer.getPosition().z)); 

        glBindVertexArray(playerVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, (sizeof(playerMesh)/sizeof(float))/8, 1); 

      

        
        // Logica di respawn - se il player è morto
        if(b_death_screen) {
            if(deathTime == 0.0f) {
                deathTime = glfwGetTime();  // Registra il momento della morte
            }
            
            float timeSinceDeath = glfwGetTime() - deathTime;
            
            // Renderizza la schermata di morte
            death_screen_rendering();
            
            // Se sono passati 5 secondi, respawna
            if(timeSinceDeath >= respawnDelay) {
                playerHP = 100.0f;
                b_death_screen = false;
                deathTime = 0.0f;
                killedByNickname = "";
                std::cout << "Respawned! HP: " << playerHP << std::endl;
            }
            return;  // Non renderizzare il resto della scena durante la morte
        }
        


        // Renderizza TUTTA l'UI in un unico frame ImGui
        render_all_ui();




        //glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.f, -5.0f));
        //model2 = glm::rotate(model2, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        //glDrawArrays(GL_TRIANGLES, 0, dim / 6);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
       glfwPollEvents();
        
    
    
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
    
    if(b_pause_menu_rendering == false && b_chat_rendering == false && b_death_screen == false){
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            player.processMovement(FORWARD, frameDeltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            player.processMovement(BACKWARD, frameDeltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            player.processMovement(LEFT, frameDeltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            player.processMovement(RIGHT, frameDeltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            player.processMovement(JUMP, frameDeltaTime);
        
        // Rilevamento sparo con mouse sinistro
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
            b_lastPressed_MOUSE_LEFT = GLFW_PRESS;
        
        if (b_lastPressed_MOUSE_LEFT == GLFW_PRESS && 
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
        {
            b_lastPressed_MOUSE_LEFT = 0;
            
            // Verifica cooldown per evitare spam
            double currentTime = glfwGetTime();
            if (currentTime - lastShootTime >= shootCooldown) {
                shoot_raycast();  // Esegui il raycast e invia lo sparo
                lastShootTime = currentTime;
            }
        }
    }

    
    if(glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS) b_lastPressed_KEY_APOSTROPHE = GLFW_PRESS;
    if (b_lastPressed_KEY_APOSTROPHE == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_APOSTROPHE) != GLFW_PRESS)
    {
        b_lastPressed_KEY_APOSTROPHE = 0;
        b_debug_menu_rendering = b_debug_menu_rendering ? false : true; 
    }

    if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !b_chat_rendering) {
        b_lastPressed_KEY_M = GLFW_PRESS;
    }
    if (b_lastPressed_KEY_M == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) != GLFW_PRESS)
    {
        b_lastPressed_KEY_M = 0;
        b_pause_menu_rendering = b_pause_menu_rendering ? false : true; 
        b_pause_menu_rendering ? emscripten_exit_pointerlock() :  emscripten_request_pointerlock("#canvas", true);
        
        //così quando si esce dal menu la camera punta nella stessa direzioen di quando si è aperto ik menu
        lastX = winWidth / 2.0f;
        lastY = winHeight / 2.0f;

    }

    // Tasto T per aprire/chiudere la chat (blocca il tasto se la chat è già attiva)
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !b_chat_rendering) {
        b_lastPressed_KEY_T = GLFW_PRESS;
    }
    if (b_lastPressed_KEY_T == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_T) != GLFW_PRESS)
    {
        b_lastPressed_KEY_T = 0;
        b_chat_rendering = !b_chat_rendering;
        
        // Reset del focus quando apri la chat per garantire autofocus
        if (b_chat_rendering) {
            chatInputFocused = false;
        }
    }

    // Tasto TAB per mostrare lo scoreboard (hold to show)
    if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        b_scoreboard_rendering = true;
    } else {
        b_scoreboard_rendering = false;
    }

    if ( glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        emscripten_request_pointerlock("#canvas", true);
        emscripten_request_fullscreen("#canvas", true);
    }
      
}


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
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    player.processMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    player.camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void shoot_raycast() {
    // 1. Ottieni la posizione e direzione dalla camera del giocatore
    glm::vec3 shooterPos = player.camera.Position;
    glm::vec3 shootDirection = player.camera.Front;  // Front è la direzione in cui guarda la camera
    Collider* colliders[2];

    colliders[1] = &enemyPlayer.headHitBox;
    colliders[0] = &enemyPlayer.bodyHitBox;

    if(glm::distance(shooterPos, enemyPlayer.headHitBox.m_position) <=
       glm::distance(shooterPos, enemyPlayer.bodyHitBox.m_position))
    {   
        colliders[0] = &enemyPlayer.headHitBox;
        colliders[1] = &enemyPlayer.bodyHitBox;
    }

    for(int colliderIndex = 0; colliderIndex < 2; colliderIndex++)
    { 
        // 3. Ray-AABB intersection (algoritmo di Slabs)
        float tMin = 0.0f;
        float tMax = 100.0f;  // Distanza massima del raycast

        glm::vec3 boxMin = colliders[colliderIndex]->getBoxMin();
        glm::vec3 boxMax = colliders[colliderIndex]->getBoxMax(); 
        for (int i = 0; i < 3; i++) {  // Per ogni asse (x, y, z)
            if (abs(shootDirection[i]) < 0.0001f) {
                // Raggio parallelo all'asse - controlla se è fuori dalla slab
                if (shooterPos[i] < boxMin[i] || shooterPos[i] > boxMax[i]) {
                    // Non c'è intersezione
                    tMax = -1.0f;
                    break;
                }
            } else {
                // Calcola intersezioni con i piani della slab
                float t1 = (boxMin[i] - shooterPos[i]) / shootDirection[i];
                float t2 = (boxMax[i] - shooterPos[i]) / shootDirection[i];

                if (t1 > t2) std::swap(t1, t2);  // Assicurati che t1 < t2

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax) break;  // Nessuna intersezione
            }
        }

        // 4. Verifica se c'è stata un'intersezione valida
        bool isHit = (tMin <= tMax) && (tMax >= 0.0f) && (tMin <= 100.0f);

        if (isHit) {



            // 6. Calcola il danno base
            float damage = colliders[colliderIndex]->m_type == 0 ? 100 : 40;



            // 7. Invia l'evento di sparo al server con i dati
           sendShootEvent(shooterPos, shootDirection, damage, glm::distance(player.getPosition(),enemyPlayer.getPosition()));

            damage < 100 ?  std::cout << "bodyhit! Damage: " << damage << std::endl 
                         :  std::cout << "HEADSHOT! Damage: " << damage << std::endl;
            colliderIndex = 2; // cosí esco del for
        } else {

            std::cout << colliders[colliderIndex]->m_type <<", MISS! Ray didn't intersect enemy hitbox, pos: " << colliders[colliderIndex]->getBoxMax().x << ", "<< colliders[colliderIndex]->getBoxMax().y << ", "<< colliders[colliderIndex]->getBoxMax().z << " ~ " << colliders[colliderIndex]->getBoxMin().x << ", "<< colliders[colliderIndex]->getBoxMin().y << ", "<< colliders[colliderIndex]->getBoxMin().z << std::endl;
        }
    }
}