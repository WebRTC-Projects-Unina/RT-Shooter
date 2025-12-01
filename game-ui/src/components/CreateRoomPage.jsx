import { useState, useEffect, useRef } from "react";
import { io } from "socket.io-client";
import "../styles/createRoomPage.css";

// Connetti al dispatcher
const socket = io("http://localhost:8080");

function CreateRoomPage({ nickname, onCancel, setScreen, setGameInfo }) {
  const [roomName, setRoomName] = useState("");
  const [password, setPassword] = useState("");
  const [level, setLevel] = useState("Level 1");
  const [levelImage, setLevelImage] = useState("");  // Nuovo stato per l'immagine del livello
  const roomNameRef = useRef(null); // 🆕 riferimento per autofocus
  const [error, setError] = useState("");


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
    if (!roomName.trim()) {
      setError("Room name is required");
      roomNameRef.current?.focus();
      return;
    }

    setError("");

    socket.emit("create_room", {
      name: roomName,
      password: password,
      level: level,
      creator: nickname
    });
  }

  function handleKeyDown(e) {     // 🆕 ENTER per creare
  if (e.key === "Enter") {
    createRoom();
  }
}


  useEffect(() => {
    // Aggiorna l'immagine del livello all'inizializzazione
    updateLevelImage(level);

    roomNameRef.current?.focus(); // autofocus sul campo nome stanza

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
          ref={roomNameRef}   
          type="text"
          value={roomName}
          onChange={(e) => {
            setRoomName(e.target.value);
            setError("");
          }}
          onKeyDown={handleKeyDown}
        />
        {error && (
        <div 
          className="error-message" 
          onClick={() => setError("")}   // 🆕 clic per nascondere errore
        >
          {error}
        </div>
      )}

      </div>

      <div className="form-block">
        <label>Password (optional)</label>
        <input
          type="password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
           onKeyDown={handleKeyDown}
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
