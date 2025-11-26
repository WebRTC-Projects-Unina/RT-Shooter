import { useEffect } from "react";
import { io } from "socket.io-client";

function Game({ roomID, port, nickname, onExit }) {

  useEffect(() => {
    const canvas = document.getElementById("canvas");

    // SOCKET con server figlio
    const gameSocket = io(`http://localhost:${port}`, {
      query: { roomID, nickname }
    });

    gameSocket.on("connect", () => {
      console.log("Connected to room server:", port);

      // notifico al server figlio
      gameSocket.emit("join_game", { nickname });

      // passo la socket al WASM
      window.Module.socket = gameSocket;
    });

    // LOAD WASM
    window.Module = {
      canvas: canvas,
      locateFile: (file) => "/" + file,
      socket: null
    };

    const script = document.createElement("script");
    script.src = "/test.js";
    script.async = true;
    document.body.appendChild(script);

    return () => {
      gameSocket.disconnect();
      document.body.removeChild(script);
    };
  }, []);

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
 


// SEMPLIFICAZIONE PER TESTING UI
/*
export default function Game({ roomID, port, nickname, onExit }) {
  return (
    <div style={{ width: "100vw", height: "100vh", background: "black" }}>
      <h1 style={{ color: "white" }}>
        ROOM {roomID} – PORT {port}
      </h1>

      <canvas
        id="canvas"
        width="800"
        height="600"
        style={{
          border: "3px solid white",
          display: "block",
          margin: "20px auto"
        }}
      />

      <button
        onClick={onExit}
        style={{
          position: "absolute",
          top: 20,
          right: 20,
          padding: "10px 20px",
          background: "red",
          color: "white",
          border: "none",
          borderRadius: "5px"
        }}
      >
        EXIT
      </button>
    </div>
  );
}

*/