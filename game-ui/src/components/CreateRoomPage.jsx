import { useState, useEffect } from "react";
import { io } from "socket.io-client";

// Connetti al dispatcher
const socket = io("http://localhost:8080");

function CreateRoomPage({ nickname, onCancel, setScreen, setGameInfo }) {
  const [roomName, setRoomName] = useState("");
  const [password, setPassword] = useState("");
  const [level, setLevel] = useState("Level 1");
  const [levelImage, setLevelImage] = useState("");  // Nuovo stato per l'immagine del livello

  function updateLevelImage(selectedLevel) {
  switch (selectedLevel) {
    case "Level 1":
      setLevelImage("1.jpg");
      break;
    case "Level 2":
      setLevelImage("2.jpg"); 
      break;
    case "Level 3":
      setLevelImage("3.jpg");
      break;
    default:
      setLevelImage(""); // Se non c'è selezione, nessuna immagine
  }
}

  function createRoom() {
    if (!roomName.trim()) return;

    socket.emit("create_room", {
      name: roomName,
      password: password,
      level: level,
      creator: nickname
    });
  }

  useEffect(() => {
    // Aggiorna l'immagine del livello all'inizializzazione
    updateLevelImage(level);

    socket.on("room_created", ({ roomID, port }) => {
      console.log("Room created!", roomID, "Redirecting to port:", port);

      const gameSocket = io(`http://localhost:${port}`);

      gameSocket.on("connect", () => {
        console.log("Connected to room server on port:", port);

        // Notifica al dispatcher
        socket.emit("player_joined", {
          roomID,
          nickname
        });

        // SALVA INFO PER GAME.JSX
        setGameInfo({ roomID, port });

        // VAI ALLA SCHERMATA DI GIOCO
        setScreen("game");
      });
    });

    return () => {
      socket.off("room_created");
    };
  }, []);

  return (
    <div className="create-room-page">
      <h2>Create a New Room</h2>

      <div className="form-block">
        <label>Room Name</label>
        <input
          type="text"
          value={roomName}
          onChange={(e) => setRoomName(e.target.value)}
        />
      </div>

      <div className="form-block">
        <label>Password (optional)</label>
        <input
          type="password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
        />
      </div>

      <div className="form-block">
        <label>Level</label>
        <select value={level} onChange={(e) => { setLevel(e.target.value); updateLevelImage(e.target.value); }}>
          <option>Level 1</option>
          <option>Level 2</option>
          <option>Level 3</option>
        </select>
      </div>
            {levelImage && (
        <div className="level-preview">
          <h3>Preview of {level}</h3>
          <img src={`/assets/${levelImage}`} alt={level} />
        </div>
      )}

      <div className="buttons">
        <button onClick={createRoom}>Create</button>
        <button onClick={onCancel}>Cancel</button>
      </div>
    </div>
  );
}



export default CreateRoomPage;
