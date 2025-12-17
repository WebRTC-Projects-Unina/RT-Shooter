
#ifndef EMJS_FUNCTIONS_H
#define EMJS_FUNCTIONS_H




#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../game/Player.h"
#include "renderer.h"



#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>






extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnEnemyUpdate(const char* msg) {
    std::string json(msg);
    glm::vec3 v;
    float yaw; 

    sscanf(json.c_str(),
           "{ \"x\": %f , \"y\": %f , \"z\": %f , \"yaw\": %f }",
           &v.x, &v.y, &v.z, &yaw);
    enemyPlayer.setPosition(v);
    enemyPlayer.Yaw = yaw;
}
}

EM_JS(void, sendPosizione, (const char* json), {
    if(!Module.socket) return;
    Module.socket.emit("player_update", JSON.parse(UTF8ToString(json)));
});


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
        // Quando ricevi la posizione del nemico, sai che è vivo
        Module.ccall("OnEnemyAlive", null, [], []);
        
        const json = JSON.stringify(pos);
        Module.ccall(
            "OnEnemyUpdate", // nome funzione C++
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
    
    // Quando il nemico muore
    Module.socket.on("enemy_died", (data) => {
        Module.ccall("OnEnemyDied", null, [], []);
    });

    Module.socket.on("map_load", (data) => {
        Module.ccall("OnMapLoad", null, ["number"], [data.map]);
    });


    
    // Quando il nemico ha ucciso qualcuno (incremente le sue kill)
    Module.socket.on("enemy_killed", (data) => {
        Module.ccall("OnEnemyKilled", null, [], []);
    });
    
    // Quando il nemico muore (incremente le sue death)
    Module.socket.on("enemy_death", (data) => {
        Module.ccall("OnEnemyDeath", null, [], []);
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




extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnMapLoad(float mapN) {
    mapNumber = (int)mapN;

    std::cout << "MapNumer Ricevuto: " << mapNumber << std::endl;

}
}

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

void sendVec3(glm::vec3 v, float yaw) {


    std::stringstream ss;
    ss << "{ \"x\": " << v.x << ", \"y\": " << v.y << ", \"z\": " << v.z << ", \"yaw\": " << yaw << " }";
    std::string json = ss.str();

    sendPosizione(json.c_str());
}

EM_JS(void, sendShootData, (const char* json), {
    if(!Module.socket) return;
    Module.socket.emit("player_shoot", JSON.parse(UTF8ToString(json)));
});

void sendShootEvent(glm::vec3 position, glm::vec3 direction, float damage, float distance) {
    std::stringstream ss;
    //Ad ora non vengono usati questi parametri, ma utili per anticheat server side naive
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

// Audio playback functions
EM_JS(void, playHitmarkerSound, (), {
    const hitmarkerElement = document.getElementById('hitmarkerSound');
    if (hitmarkerElement) {
        hitmarkerElement.currentTime = 0;
        hitmarkerElement.play().catch(err => console.log("Hitmarker play error:", err));
    }
});

EM_JS(void, playDamageSound, (), {
    const damageElement = document.getElementById('damageSound');
    if (damageElement) {
        damageElement.currentTime = 0;
        damageElement.play().catch(err => console.log("Damage play error:", err));
    }
});

// Invia evento di morte al server
EM_JS(void, sendPlayerDeathEvent, (), {
    if(!Module.socket) return;
    Module.socket.emit("player_death", {});
});

// Invia evento quando hai ucciso il nemico
EM_JS(void, sendPlayerKilledEvent, (), {
    if(!Module.socket) return;
    Module.socket.emit("player_killed", {});
});

#endif