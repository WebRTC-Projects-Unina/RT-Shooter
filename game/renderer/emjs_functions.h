
#ifndef EMJS_FUNCTIONS_H
#define EMJS_FUNCTIONS_H




#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../game/Player.h"




#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>






extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnMessageFromJS(const char* msg) {
    std::string json(msg);
    glm::vec3 v;

    // Parsing (rozzo ma funziona per il demo)
    sscanf(json.c_str(),
           "{ \"x\": %f , \"y\": %f , \"z\": %f }",
           &v.x, &v.y, &v.z);
    enemyPlayer.setPosition(v);
}
}


extern "C" {
EMSCRIPTEN_KEEPALIVE
void exitJSCallback() {
    emscripten_force_exit(0);
}
}
EM_JS(void, RegisterSocketIOCallback, (), {
    if (!Module.socket) {
        console.log("Socket non pronta!");
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
    
    // Quando il nemico si connette
    Module.socket.on("enemy_joined", (data) => {
        Module.ccall(
            "OnEnemyJoined",
            null,
            ["string"],
            [data.nickname]
        );
    });
    
    // Gestione messaggi di chat in arrivo
    Module.socket.on("chat_message_received", (data) => {
        Module.ccall(
            "OnChatMessageReceived",
            null,
            ["string", "string"],
            [data.message, data.senderNickname]
        );
    });
    
    // Quando il nemico spara - calcola il danno lato client
    Module.socket.on("player_shot", (data) => {
        const damage = (data.damage !== undefined) ? data.damage : 25;  // Permette 0 danno se miss
        const shooterNickname = data.shooterNickname || "Unknown";
        Module.ccall(
            "OnEnemyShot",
            null,
            ["number", "string"],
            [damage, shooterNickname]
        );
    });
});



EM_JS(void, sendPosizione, (const char* json), {
    // jsSocket deve essere la tua WebSocket aperta in JavaScript
    if(!Module.socket) return;
    Module.socket.emit("player_update", JSON.parse(UTF8ToString(json)));
});


EM_JS(void, setPlayerDistance, (float distanza), {
    //(vedere screen desmos)
    let j = 0.5;
    let p = 1.6;
    if(distanza >= -j && distanza <= j) remoteAudio.volume = 1;
    else remoteAudio.volume = 1 / Math.pow(distanza/j, p);
    
});

EM_JS(void, exitGame, (), {
    if(!Module) return; 
    Module.exitGame();
});

void sendVec3(glm::vec3 v) {


    std::stringstream ss;
    ss << "{ \"x\": " << v.x << ", \"y\": " << v.y << ", \"z\": " << v.z << " }";
    std::string json = ss.str();

    sendPosizione(json.c_str());
}

EM_JS(void, sendShootData, (const char* json), {
    if(!Module.socket) return;
    Module.socket.emit("player_shoot", JSON.parse(UTF8ToString(json)));
});

void sendShootEvent(glm::vec3 position, glm::vec3 direction, float damage, float distance) {
    std::stringstream ss;
    ss << "{ "
       << "\"pos\": { \"x\": " << position.x << ", \"y\": " << position.y << ", \"z\": " << position.z << " }, "
       << "\"dir\": { \"x\": " << direction.x << ", \"y\": " << direction.y << ", \"z\": " << direction.z << " }, "
       << "\"damage\": " << damage << ", "
       << "\"distance\": " << distance
       << " }";
    std::string json = ss.str();
    
    sendShootData(json.c_str());
}
// Funzioni per la chat
EM_JS(void, sendChatData, (const char* json), {
    if(!Module.socket) return;
    Module.socket.emit("chat_message", JSON.parse(UTF8ToString(json)));
});
// Invia un messaggio di chat al server
void sendChatMessage(const char* message) {
    std::stringstream ss;
    ss << "{ \"message\": \"" << message << "\" }";
    std::string json = ss.str();
    
    sendChatData(json.c_str());
}

EM_JS(int, canvas_get_width, (), {
  
  const canvasElement = document.querySelector('#canvas');
  if(!canvasElement) return 1280;
  canvasElement.width = window.innerWidth;
  return canvasElement.width;
});

EM_JS(int, canvas_get_height, (), {
    const canvasElement = document.querySelector('#canvas');  
     if(!canvasElement) return 720;
    canvasElement.height =  window.innerHeight;
    return canvasElement.height;
});

#endif