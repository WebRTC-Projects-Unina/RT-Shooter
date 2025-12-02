import { useEffect } from "react";
import { io } from "socket.io-client";
import socket from "../index";


function Game({ roomID, port, nickname, onExit }) {
   // creo la socket
    const gameSocket = io(`http://localhost:${port}`, {
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

      // 👇 SE il WASM l'ha già caricato → lo chiamiamo ORA
      if (window.Module.RegisterSocketIOCallback) {
        console.log("🔵 React chiama RegisterSocketIOCallback (socket pronta)");
        window.Module.RegisterSocketIOCallback();
      }

    });

    // Carico il WASM

   

    script.onload = () => {
      console.log("WASM caricato");

      // 👇 Se la socket è già pronta → registra i listener
      if (window.Module.socket && window.Module.RegisterSocketIOCallback) {
        console.log("🟢 React chiama RegisterSocketIOCallback (wasm caricato)");
        window.Module.RegisterSocketIOCallback();
      }
    };

    document.body.appendChild(script);

    // 🔴 IMPORTANTISSIMO: CLEANUP
    return () => {
      console.log("Cleanup Game.jsx");
      gameSocket.disconnect();
      document.body.removeChild(script);
    };

  }, []); // CHIUDE useEffect 🔥🔥🔥


  return (
    <div className="game-screen">
      <canvas
        id="canvas"
        style={{ width: "100vw", height: "100vh" }}
      />

      <button className="exit-btn" onClick={onExit}>
        EXIT
      </button>
    </div>
  );
}

export default Game;
