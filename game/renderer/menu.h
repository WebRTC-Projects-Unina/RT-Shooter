#ifndef MENU_H
#define MENU_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

bool b_debug_menu_rendering = false;
bool b_pause_menu_rendering = false;

void debug_menu_rendering(glm::vec3 playerPos, glm::vec3 enemyPosition) {
            //Inizia il frame ImGui
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        //ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        //ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //Disegna la GUI con ImGui
        ImGui::Begin("Debug Menu", NULL, window_flags);
            
        
        //draw_list->AddLine(ImVec2(center.x, center.y-25), ImVec2(center.x, center.y+5), col, th);                          
        //draw_list->AddLine(ImVec2(center.x-5, center.y), ImVec2(center.x+5, center.y), col, th);
        
        

        ImGui::InputFloat3("Posizione", (float*)&playerPos);
        ImGui::InputFloat3("Enemy Pos", (float*)&enemyPosition);


            

        ImGui::End();

        
        //Rendering ImGui
            ImGui::Render();  // Chiude il frame ImGui e prepara i dati di disegno
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  
}


void pause_menu_rendering() {
                    //Inizia il frame ImGui
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

        ImGui::Begin("Debug Menu");
            
        

            


         if (b_pause_menu_rendering && ImGui::Button("Chiudi Menu"))
            b_pause_menu_rendering = false;


        
        ImGui::End();
  
        //Rendering ImGui
            ImGui::Render();  // Chiude il frame ImGui e prepara i dati di disegno
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  
}


#endif
