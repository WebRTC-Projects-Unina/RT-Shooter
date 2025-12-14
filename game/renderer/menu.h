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
#include <cstdio>     // Per std::snprintf

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
bool b_match_over = false;      // true quando la partita è finita
bool b_player_won = false;      // true se questo client ha vinto
float matchEndTime = 0.0f;      // tempo in cui è terminata la partita
float matchEndDuration = 15.0f; // durata della schermata finale prima del ritorno alla lobby
bool b_match_end_exit_sent = false; // evita chiamate multiple a exitGame

// Forward declarations per la logica di fine partita
void TriggerMatchEnd(bool playerWon);
void CheckWinCondition();

// Variabili per l'HP
float playerHP = 100.0f;
float enemyHP = 100.0f;
std::string killedByNickname = "";  // Nome di chi ci ha ucciso
bool b_death_screen = false;  // True quando il player è morto
bool b_enemy_alive = true;  // True quando il nemico è vivo, false quando è morto
float enemyDeathTime = 0.0f;  // Traccia quando il nemico è morto
float enemyRespawnDelay = 5.0f;  // 5 secondi prima che il nemico possa riapparire
float hitFeedbackTimer = 0.0f;   // Timer breve per il feedback visivo dell'hit
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

// Callback quando il nemico spara (calcola il danno lato client)
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyShot(float damage, const char* shooterNickname) {
    playerHP -= damage;
    std::cout << "Sono stato HITTATO! Danno ricevuto: " << damage << " HP rimanenti: " << playerHP << std::endl;
    
    // Riproduci suono di danno
    playDamageSound();
    
    // Se il player muore
    if (playerHP <= 0.0f) {
        playerHP = 0.0f;
        b_death_screen = true;
        playerDeaths++;
        killedByNickname = std::string(shooterNickname);  // Salva il nome di chi ci ha ucciso
        std::cout << "Killed by: " << killedByNickname << std::endl;
        sendPlayerDeathEvent();  // Notifica al server che sei morto
    }
}
}

// Callback quando il nemico muore
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyDied() {
    b_enemy_alive = false;
    enemyDeathTime = glfwGetTime();  // Registra il tempo di morte
    playerKills++;  // Incremente le kill localmente
    std::cout << "You killed the enemy! Total kills: " << playerKills << std::endl;
    sendPlayerKilledEvent();  // Manda evento al server per sincronizzare
    CheckWinCondition();
}
}

// Callback quando il nemico respawna (riceve la posizione significa che è vivo)
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyAlive() {
    // Ignora gli update per 5 secondi dopo la morte
    if (enemyDeathTime > 0.0f) {
        float timeSinceDeath = glfwGetTime() - enemyDeathTime;
        if (timeSinceDeath < enemyRespawnDelay) {
            // Still in the 5-second death window, ignore this update
            return;
        }
        // 5 secondi sono passati, il nemico può riapparire
        enemyDeathTime = 0.0f;
    }
    
    b_enemy_alive = true;
}
}

// Callback quando il nemico ha ucciso qualcuno (incremente le sue kill)
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyKilled() {
    enemyKills++;
    std::cout << "Enemy got a kill! Total: " << enemyKills << std::endl;
    CheckWinCondition();
}
}

// Callback quando il nemico muore (incremente le sue death)
extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyDeath() {
    enemyDeaths++;
    std::cout << "Enemy died! Total deaths: " << enemyDeaths << std::endl;
}
}


void TriggerMatchEnd(bool playerWon) {
    if (b_match_over) return;

    b_match_over = true;
    b_player_won = playerWon;
    matchEndTime = glfwGetTime();
    b_scoreboard_rendering = true;   // scoreboard sempre visibile durante la schermata finale
    b_death_screen = false;          // disattiva eventuale death screen in corso
    b_pause_menu_rendering = false;  // evita menu pausa durante la fine match
    player.setVelocity(glm::vec3(0.0f));
}

void CheckWinCondition() {
    if (b_match_over) return;

    if (playerKills >= 10) {
        TriggerMatchEnd(true); // player ha vinto
    } else if (enemyKills >= 10) {
        TriggerMatchEnd(false); // nemico ha vinto
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

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::Begin("Debug Menu", NULL, window_flags);
        ImGui::InputFloat3("Posizione", (float*)&playerPos);
        ImGui::InputFloat3("Enemy Pos", (float*)&enemyPosition);


            

        ImGui::End();
}


void crosshair_rendering() {
    // ===== CROSSHAIR =====
    if (!b_pause_menu_rendering && !b_chat_rendering && !b_match_over) {
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));
        ImGui::Begin("Crosshair", NULL, window_flags);
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(winWidth * 0.5f, winHeight * 0.5f);
        
        ImU32 col_normal = IM_COL32(255, 255, 255, 255);
        ImU32 col_hit    = IM_COL32(0, 200, 255, 255); // Azzurro per feedback positivo
        float thickness = 1.5f;
        float inner_line_length = 10.0f;
        float inner_line_offset = 0.0f;  // Parte dal centro

        // Aggiorna il timer di feedback se attivo
        if (hitFeedbackTimer > 0.0f) {
            hitFeedbackTimer = std::max(0.0f, hitFeedbackTimer - ImGui::GetIO().DeltaTime);
        }

        const bool isHitFlash = hitFeedbackTimer > 0.0f;
        ImU32 col = isHitFlash ? col_hit : col_normal;

        if (isHitFlash) {
            // Mirino ruotato (a X) per il feedback dell'hit
            draw_list->AddLine(ImVec2(center.x - inner_line_length, center.y - inner_line_length), center, col, thickness);
            draw_list->AddLine(center, ImVec2(center.x + inner_line_length, center.y + inner_line_length), col, thickness);
            draw_list->AddLine(ImVec2(center.x - inner_line_length, center.y + inner_line_length), center, col, thickness);
            draw_list->AddLine(center, ImVec2(center.x + inner_line_length, center.y - inner_line_length), col, thickness);
        } else {
            // Mirino classico a croce
            draw_list->AddLine(ImVec2(center.x, center.y - inner_line_offset - inner_line_length),
                              ImVec2(center.x, center.y - inner_line_offset), col, thickness);
            draw_list->AddLine(ImVec2(center.x, center.y + inner_line_offset),
                              ImVec2(center.x, center.y + inner_line_offset + inner_line_length), col, thickness);
            draw_list->AddLine(ImVec2(center.x - inner_line_offset - inner_line_length, center.y),
                              ImVec2(center.x - inner_line_offset, center.y), col, thickness);
            draw_list->AddLine(ImVec2(center.x + inner_line_offset, center.y),
                              ImVec2(center.x + inner_line_offset + inner_line_length, center.y), col, thickness);
        }
        
        draw_list->AddCircleFilled(center, 0.5f, col, 4);

        ImGui::End();
    }
}

void health_bar_rendering() {
    // ===== HEALTH BAR =====
    if (!b_pause_menu_rendering) {
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));
        ImGui::Begin("HealthBar", NULL, window_flags);
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        float barWidth = 300.0f;
        float barHeight = 25.0f;
        float barX = (winWidth - barWidth) * 0.5f;
        float barY = winHeight - 60.0f;
        
        ImU32 col_bg = IM_COL32(0, 0, 0, 150);
        draw_list->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barWidth, barY + barHeight), col_bg, 3.0f);
        
        float hpPercent = playerHP / 100.0f;
        if (hpPercent < 0.0f) hpPercent = 0.0f;
        if (hpPercent > 1.0f) hpPercent = 1.0f;
        
        float fillWidth = barWidth * hpPercent;
        
        ImU32 col_hp;
        if (hpPercent > 0.5f) {
            float t = (hpPercent - 0.5f) * 2.0f;
            col_hp = IM_COL32(255 * (1.0f - t), 255, 0, 255);
        } else {
            float t = hpPercent * 2.0f;
            col_hp = IM_COL32(255, 255 * t, 0, 255);
        }
        
        if (fillWidth > 0) {
            draw_list->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + fillWidth, barY + barHeight), col_hp, 3.0f);
        }
        
        ImU32 col_border = IM_COL32(255, 255, 255, 255);
        draw_list->AddRect(ImVec2(barX, barY), ImVec2(barX + barWidth, barY + barHeight), col_border, 3.0f, 0, 2.0f);
        
        char hpText[32];
        sprintf(hpText, "%.0f / 100", playerHP);
        ImVec2 textSize = ImGui::CalcTextSize(hpText);
        ImVec2 textPos = ImVec2(barX + (barWidth - textSize.x) * 0.5f, barY + (barHeight - textSize.y) * 0.5f);
        draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), hpText);

        ImGui::End();
    }
}


void chat_rendering() {
    // ===== CHAT =====
    {
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoCollapse;
        
        if (!b_chat_rendering) {
            window_flags |= ImGuiWindowFlags_NoInputs;
        }
        
        float chatWidth = 400.0f;
        float chatHeight = 300.0f;
        float margin = 20.0f;
        
        ImGui::SetNextWindowPos(ImVec2(margin, winHeight - chatHeight - margin));
        ImGui::SetNextWindowSize(ImVec2(chatWidth, chatHeight));
        
        float bgAlpha = b_chat_rendering ? 0.85f : 0.3f;
        ImGui::SetNextWindowBgAlpha(bgAlpha);
        
        ImGui::Begin("Chat", NULL, window_flags);
        
        float inputHeight = b_chat_rendering ? -30 : 0;
        ImGui::BeginChild("ChatMessages", ImVec2(0, inputHeight), false);
        
        for (const auto& msg : chatMessages) {
            ImVec4 color = msg.isMine ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            ImGui::TextColored(color, "%s", msg.text.c_str());
        }
        
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        
        ImGui::EndChild();
        
        if (b_chat_rendering) {
            ImGui::Separator();
            
            if (!chatInputFocused) {
                ImGui::SetKeyboardFocusHere();
                chatInputFocused = true;
            }
            
            if (ImGui::InputText("##ChatInput", chatInputBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(chatInputBuffer) > 0) {
                    ChatMessage myMsg;
                    myMsg.text = "[" + playerNickname + "]: " + std::string(chatInputBuffer);
                    myMsg.isMine = true;
                    myMsg.timestamp = glfwGetTime();
                    chatMessages.push_back(myMsg);
                    
                    sendChatMessage(chatInputBuffer);
                }
                
                memset(chatInputBuffer, 0, sizeof(chatInputBuffer));
                b_chat_rendering = false;
                chatInputFocused = false;
            }
        } else {
            if (chatInputFocused) {
                chatInputFocused = false;
                memset(chatInputBuffer, 0, sizeof(chatInputBuffer));
            }
        }
        
        ImGui::End();
    }
}


void scoreboard_rendering() {
    
    // ===== SCOREBOARD =====
    if (b_scoreboard_rendering || b_match_over) {
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoBackground;

        float boardWidth = 500.0f;
        float boardHeight = 200.0f;
        
        ImVec2 center = ImVec2(winWidth * 0.5f, winHeight * 0.5f);
        ImGui::SetNextWindowPos(ImVec2(center.x - boardWidth * 0.5f, center.y - boardHeight * 0.5f));
        ImGui::SetNextWindowSize(ImVec2(boardWidth, boardHeight));
        ImGui::SetNextWindowBgAlpha(0.9f);
        
        ImGui::Begin("Scoreboard", NULL, window_flags);
        
        ImGui::SetCursorPosX((boardWidth - ImGui::CalcTextSize("SCOREBOARD").x) * 0.5f);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SCOREBOARD");
        ImGui::Separator();
        ImGui::Spacing();
        
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
        
        std::sort(players.begin(), players.end(), [](const PlayerStats& a, const PlayerStats& b) {
            return a.kills > b.kills;
        });
        
        if (ImGui::BeginTable("ScoreTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Nickname", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Kills", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Deaths", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();
            
            for (const auto& player : players) {
                ImGui::TableNextRow();
                
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
    }
}


void pause_menu_rendering() {
                    //Inizia il frame ImGui
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        
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
}

void death_screen_rendering() {
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Schermata di morte a schermo pieno semi-trasparente
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));
    ImGui::SetNextWindowBgAlpha(0.7f);  // Sfondo scuro semitrasparente
    
    ImGui::Begin("DeathScreen", NULL, window_flags);
    
    // Testo centrato "YOU DIED"
    const char* deathText = "YOU DIED";
    ImVec2 deathTextSize = ImGui::CalcTextSize(deathText);
    ImGui::SetCursorPosX((winWidth - deathTextSize.x) * 0.5f);
    ImGui::SetCursorPosY(winHeight * 0.35f);
    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", deathText);  // Rosso
    
    // "Killed by [Nickname]"
    std::string killedByText = "Killed by " + killedByNickname;
    ImVec2 killedBySize = ImGui::CalcTextSize(killedByText.c_str());
    ImGui::SetCursorPosX((winWidth - killedBySize.x) * 0.5f);
    ImGui::SetCursorPosY(winHeight * 0.45f);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), "%s", killedByText.c_str());
    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void match_end_rendering() {
    if (!b_match_over) return;

    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground;

    float elapsed = glfwGetTime() - matchEndTime;
    float remaining = matchEndDuration - elapsed;
    if (remaining < 0.0f) remaining = 0.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));
    ImGui::SetNextWindowBgAlpha(0.75f);

    ImGui::Begin("MatchEndScreen", NULL, window_flags);

    const char* endText = b_player_won ? "HAI VINTO" : "HAI PERSO";
    ImVec4 endColor = b_player_won ? ImVec4(0.2f, 1.0f, 0.2f, 1.0f) : ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    ImVec2 endTextSize = ImGui::CalcTextSize(endText);
    ImGui::SetCursorPosX((winWidth - endTextSize.x) * 0.5f);
    ImGui::SetCursorPosY(winHeight * 0.35f);
    ImGui::TextColored(endColor, "%s", endText);

    char timerBuf[64];
    int remainingSeconds = static_cast<int>(remaining + 0.999f);
    if (remainingSeconds < 0) remainingSeconds = 0;
    std::snprintf(timerBuf, sizeof(timerBuf), "Ritorno alla lobby in %d s", remainingSeconds);
    ImVec2 timerSize = ImGui::CalcTextSize(timerBuf);
    ImGui::SetCursorPosX((winWidth - timerSize.x) * 0.5f);
    ImGui::SetCursorPosY(winHeight * 0.60f);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), "%s", timerBuf);

    ImGui::End();

    if (remaining <= 0.0f && !b_match_end_exit_sent) {
        b_match_end_exit_sent = true;
        exitGame();
    }
}

// FUNZIONE UNIFICATA per tutto l'UI ImGui
void render_all_ui() {
    // Inizia il frame ImGui UNA SOLA VOLTA
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    

    chat_rendering();
    crosshair_rendering();
    health_bar_rendering();
    scoreboard_rendering();
    match_end_rendering();
    if(b_debug_menu_rendering) debug_menu_rendering(player.getPosition(), enemyPlayer.getPosition());
    if(b_pause_menu_rendering) {pause_menu_rendering(); b_debug_menu_rendering = false;}
   
    
    
    // Fine del frame ImGui - Render UNA SOLA VOLTA
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


#endif
