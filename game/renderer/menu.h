#ifndef MENU_H
#define MENU_H

#include <emscripten.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h> 
#include "emjs_functions.h"
#include <vector>
#include <string>
#include <algorithm>  // Per std::sort

bool b_debug_menu_rendering = false;
bool b_pause_menu_rendering = false;
bool b_chat_rendering = false;
bool b_scoreboard_rendering = false;  // Scoreboard con TAB

// Variabili per la chat
struct ChatMessage {
    std::string text;
    bool isMine;  // true = mio messaggio, false = messaggio nemico
    float timestamp;
};

std::vector<ChatMessage> chatMessages;
char chatInputBuffer[256] = "";
bool chatInputFocused = false;
std::string playerNickname = "";  // Vuoto inizialmente, viene settato da SetPlayerNickname

// Variabili per lo scoreboard
int playerKills = 0;
int playerDeaths = 0;
std::string enemyNickname = "";
int enemyKills = 0;
int enemyDeaths = 0;
// Funzione chiamata da JS quando arriva un messaggio di chat
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnChatMessageReceived(const char* message, const char* senderNickname) {
    ChatMessage msg;
    msg.text = "[" + std::string(senderNickname) + "]: " + std::string(message);
    msg.isMine = false;
    msg.timestamp = glfwGetTime();
    chatMessages.push_back(msg);
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void SetPlayerNickname(const char* nickname) {
    playerNickname = std::string(nickname);
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyJoined(const char* nickname) {
    enemyNickname = std::string(nickname);
}
}


void imgui_style_setup(){
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 10.0f;  
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.12f, 0.13f, 0.19f, 0.95f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.40f, 0.95f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.28f, 0.32f, 0.77f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_InputTextCursor]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
    colors[ImGuiCol_TabSelected]            = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TabDimmed]              = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink]               = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_TreeLines]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavCursor]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}




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


void crosshair_rendering() {
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoInputs;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    float centerX = winWidth / 2.0f;
    float centerY = winHeight / 2.0f;
    
    ImGui::SetNextWindowPos(ImVec2(centerX - 30, centerY - 30));
    ImGui::SetNextWindowSize(ImVec2(60, 60));
    
    ImGui::Begin("Crosshair", NULL, window_flags);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center(centerX, centerY);
    
    ImU32 col_white = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Parametri dal codice Valorant decodificato
    float inner_line_length = 10.0f;   // l;4 
    float inner_line_offset = 0.0f;   // o;0 (inizia dal centro)
    float thickness = 1.5f;            // b;0 (boldness minima, ma visibile)
    
    // Linea verticale (top)
    draw_list->AddLine(ImVec2(center.x, center.y - inner_line_offset - inner_line_length),
                       ImVec2(center.x, center.y - inner_line_offset), col_white, thickness);
    // Linea verticale (bottom)
    draw_list->AddLine(ImVec2(center.x, center.y + inner_line_offset),
                       ImVec2(center.x, center.y + inner_line_offset + inner_line_length), col_white, thickness);
    // Linea orizzontale (left)
    draw_list->AddLine(ImVec2(center.x - inner_line_offset - inner_line_length, center.y),
                       ImVec2(center.x - inner_line_offset, center.y), col_white, thickness);
    // Linea orizzontale (right)
    draw_list->AddLine(ImVec2(center.x + inner_line_offset, center.y),
                       ImVec2(center.x + inner_line_offset + inner_line_length, center.y), col_white, thickness);
    
    // Punto centrale piccolo
    draw_list->AddCircleFilled(center, 0.5f, col_white, 4);

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void chat_rendering(bool isActive) {
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoCollapse;
    
    // Se la chat non è attiva, disabilita l'input
    if (!isActive) {
        window_flags |= ImGuiWindowFlags_NoInputs;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Posiziona la chat in basso a sinistra
    float chatWidth = 400.0f;
    float chatHeight = 300.0f;
    float margin = 20.0f;
    
    ImGui::SetNextWindowPos(ImVec2(margin, winHeight - chatHeight - margin));
    ImGui::SetNextWindowSize(ImVec2(chatWidth, chatHeight));
    
    // Trasparenza dinamica: più trasparente se non attiva
    float bgAlpha = isActive ? 0.85f : 0.3f;
    ImGui::SetNextWindowBgAlpha(bgAlpha);
    
    ImGui::Begin("Chat", NULL, window_flags);
    
    // Area messaggi (scrollabile)
    float inputHeight = isActive ? -30 : 0;  // Nascondi input se non attiva
    ImGui::BeginChild("ChatMessages", ImVec2(0, inputHeight), true);
    
    // Trasparenza testo: più trasparente se non attiva
    float textAlpha = isActive ? 1.0f : 0.5f;
    // Visualizza i messaggi
    for (const auto& msg : chatMessages) {
        if (msg.isMine) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, textAlpha)); // Verde
            ImGui::Text("[%s]: %s", playerNickname.c_str(), msg.text.c_str());
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, textAlpha)); // Rosso
            ImGui::Text("%s", msg.text.c_str());
        }
        ImGui::PopStyleColor();
    }
    
    // Auto-scroll verso il basso quando arrivano nuovi messaggi
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    
    ImGui::EndChild();
    
    // Input box visibile SOLO se la chat è attiva
    if (isActive) {
        ImGui::Separator();
        
        // Focus automatico quando la chat diventa attiva
        if (!chatInputFocused) {
            ImGui::SetKeyboardFocusHere();
            chatInputFocused = true;
        }
        
        // Input text con callback per Invio
        if (ImGui::InputText("##ChatInput", chatInputBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Messaggio inviato quando premi Invio
            if (strlen(chatInputBuffer) > 0) {
                // Aggiungi il messaggio alla lista locale
                ChatMessage myMsg;
                myMsg.text = std::string(chatInputBuffer);
                myMsg.isMine = true;
                myMsg.timestamp = glfwGetTime();
                chatMessages.push_back(myMsg);
                
                // Invia il messaggio al server tramite socket
                sendChatMessage(chatInputBuffer);
            }
            
            // Pulisci l'input e chiudi la chat (Invio sempre chiude)
            memset(chatInputBuffer, 0, sizeof(chatInputBuffer));
            b_chat_rendering = false;
            chatInputFocused = false;
        }
    } else {
        // Se la chat viene chiusa (non attiva), resetta lo stato
        if (chatInputFocused) {
            chatInputFocused = false;
            memset(chatInputBuffer, 0, sizeof(chatInputBuffer));
        }
    }
    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void scoreboard_rendering() {
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoInputs |  // Non blocca input di gioco
        ImGuiWindowFlags_NoBackground;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dimensioni scoreboard
    float boardWidth = 500.0f;
    float boardHeight = 200.0f;
    
    // Centra lo scoreboard
    ImVec2 center = ImVec2(winWidth * 0.5f, winHeight * 0.5f);
    ImGui::SetNextWindowPos(ImVec2(center.x - boardWidth * 0.5f, center.y - boardHeight * 0.5f));
    ImGui::SetNextWindowSize(ImVec2(boardWidth, boardHeight));
    ImGui::SetNextWindowBgAlpha(0.9f);  // Sfondo semi-trasparente
    
    ImGui::Begin("Scoreboard", NULL, window_flags);
    
    // Titolo centrato
    ImGui::SetCursorPosX((boardWidth - ImGui::CalcTextSize("SCOREBOARD").x) * 0.5f);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SCOREBOARD");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Prepara i dati per la tabella (ordina per kills)
    struct PlayerStats {
        std::string nickname;
        int kills;
        int deaths;
        bool isMe;
    };
    
    std::vector<PlayerStats> players;
    players.push_back({playerNickname.empty() ? "You" : playerNickname, playerKills, playerDeaths, true});
    if (!enemyNickname.empty()) {
        players.push_back({enemyNickname, enemyKills, enemyDeaths, false});
    }
    
    // Ordina per kills (decrescente)
    std::sort(players.begin(), players.end(), [](const PlayerStats& a, const PlayerStats& b) {
        return a.kills > b.kills;
    });
    
    // Tabella con 3 colonne: Nickname | Kills | Deaths
    if (ImGui::BeginTable("ScoreTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Header
        ImGui::TableSetupColumn("Nickname", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Kills", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Deaths", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableHeadersRow();
        
        // Righe dei player
        for (const auto& player : players) {
            ImGui::TableNextRow();
            
            // Colore verde per te, rosso per nemico
            ImVec4 color = player.isMe ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(color, "%s", player.nickname.c_str());
            
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(color, "%d", player.kills);
            
            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(color, "%d", player.deaths);
        }
        
        ImGui::EndTable();
    }
    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void pause_menu_rendering() {
                    //Inizia il frame ImGui
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));
       
        ImGui::Begin("Pause Menu",  NULL, window_flags);
        
 ImVec2 menuDim = ImGui::GetContentRegionAvail();

            float buttonWidth  = menuDim.x * 0.25f;  
            float buttonHeight = menuDim.x * 0.0625f; 


            float offsetX = (menuDim.x - buttonWidth) * 0.5f;
            float offsetY = (menuDim.y - buttonHeight) * 0.5f;
            

            ImGui::SetCursorPosX(offsetX);
            ImGui::SetCursorPosY(offsetY - buttonHeight/2 );

            if (b_pause_menu_rendering && ImGui::Button("Riprendi Partita", ImVec2(buttonWidth, buttonHeight))){
                emscripten_request_pointerlock("#canvas", true);
                b_pause_menu_rendering = false;
            }
            ImGui::SetCursorPosX(offsetX);
            ImGui::SetCursorPosY(offsetY + buttonHeight);

             if (ImGui::Button("Ritorna alla selezione delle Stanze", ImVec2(buttonWidth, buttonHeight))){
                exitGame();
            }
            
        ImGui::End();
        
        //Rendering ImGui
            ImGui::Render();  // Chiude il frame ImGui e prepara i dati di disegno
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  
}


#endif
