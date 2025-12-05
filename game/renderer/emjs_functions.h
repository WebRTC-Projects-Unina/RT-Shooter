
#ifndef EMJS_FUNCTIONS_H
#define EMJS_FUNCTIONS_H




#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


#include <fstream>
#include <string>
#include <sstream>






extern "C" {
EMSCRIPTEN_KEEPALIVE
void OnMessageFromJS(const char* msg) {
    std::string json(msg);
    glm::vec3 v;

    // Parsing (rozzo ma funziona per il demo)
    sscanf(json.c_str(),
           "{ \"x\": %f , \"y\": %f , \"z\": %f }",
           &v.x, &v.y, &v.z);

    enemyPosition = v;
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
});



EM_JS(void, sendPosizione, (const char* json), {
    // jsSocket deve essere la tua WebSocket aperta in JavaScript
    if(!Module.socket) return;
    Module.socket.emit("player_update", JSON.parse(UTF8ToString(json)));
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