import { useEffect } from "react";
import { io } from "socket.io-client";
import socket from "../index";



function Game({ roomID, port, nickname, onExit }) {
   // creo la socket
    const gameSocket = io(`http://${window.location.hostname}:${port}`, {
      query: { roomID, nickname }
    });
  useEffect(() => {
    const canvas = document.getElementById("canvas");

    // creo Module
    window.Module = {
      canvas: canvas,
      locateFile: (file) => "/" + file,
      socket: null,
      RegisterSocketIOCallback: null
    };

       const script = document.createElement("script");
 

    gameSocket.on("connect", () => {
      console.log("Connected to room server:", port);
        script.src = "/test.js";
        script.async = true;

      gameSocket.emit("join_game", { nickname });

      window.Module.socket = gameSocket;

 
      if (window.Module.RegisterSocketIOCallback) {
        window.Module.RegisterSocketIOCallback();
      }

    });

    
   

    script.onload = () => {
      console.log("WASM caricato");

      
      if (window.Module.socket && window.Module.RegisterSocketIOCallback) {
        window.Module.RegisterSocketIOCallback();
      }
    };

    document.body.appendChild(script);

    return () => {
      console.log("Cleanup Game.jsx");
      gameSocket.disconnect();
      document.body.removeChild(script);
    };

  }, []); 


  return (
    <div className="game-screen">
      <canvas
        id="canvas"
      />
    </div>
  );
}

export default Game;
