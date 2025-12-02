import { useState, useEffect, useRef } from "react";
import "../styles/createRoomPage.css";
import socket from "../DispatcherSocket";


// Connetti al dispatcher


function CreateRoomPage({ nickname, onCancel, setScreen, setGameInfo}) {
  const [roomName, setRoomName] = useState("");
  const [password, setPassword] = useState("");
  const [level, setLevel] = useState("Level 1");
  const [levelImage, setLevelImage] = useState("");  
  const roomNameRef = useRef(null); 
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

  function handleKeyDown(e) {     
  if (e.key === "Enter") {
    createRoom();
  }
}


  useEffect(() => {
    updateLevelImage(level);

    roomNameRef.current?.focus(); 

    socket.on("room_created", ({ roomID, port }) => {
      console.log("Room created!", roomID, "Redirecting to port:", port);

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
          onClick={() => setError("")}   
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

      <div className="select-wrapper">
        <select 
          value={level} 
          onChange={(e) => { 
            setLevel(e.target.value); 
            updateLevelImage(e.target.value); 
          }}
        >
          <option>Level 1</option>
          <option>Level 2</option>
          <option>Level 3</option>
        </select>
      </div>
    </div>

            {levelImage && (
            <div className="form-block">
              <label>Preview of {level}</label>

              <div className="level-preview-box">
                <img src={`/assets/${levelImage}`} alt={level} />
              </div>
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
