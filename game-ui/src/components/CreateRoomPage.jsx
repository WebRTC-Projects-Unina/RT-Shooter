import { useState } from "react";
import { io } from "socket.io-client";
import { useEffect } from "react";


// Connetti al dispatcher
const socket = io("http://localhost:8080");

function CreateRoomPage({nickname, onCancel, onCreate }) {
  const [roomName, setRoomName] = useState("");
  const [password, setPassword] = useState("");
  const [level, setLevel] = useState("Level 1");
  const [connectedToRoomServer, setConnectedToRoomServer] = useState(false);
  const [roomServerPort, setRoomServerPort] = useState(null);

  function createRoom() {
    if (!roomName.trim()) return;

    // INVIO AL SERVER
    socket.emit("create_room", {
      name: roomName,
      password: password,
      level: level,
      creator: nickname
    });

    
  }

    useEffect(() => {
    socket.on("room_created", ({ roomID, port }) => {
      console.log("Room created!", roomID, "Redirecting to port:", port);

      // Connessione al room server dedicato
      const gameSocket = io(`http://localhost:${port}`);

      gameSocket.on("connect", () => {
        console.log("Connected to room server on port:", port);
        setConnectedToRoomServer(true);
        setRoomServerPort(port);
         socket.emit("player_joined", { // notifica al dispatcher
          roomID,
          nickname
        });

        // Torna alla lobby (non serve passare dati locali)
      onCreate();
      });
    });

    // cleanup quando il componente viene smontato
    return () => {
      socket.off("room_created");
    };
   }, []);

  return (
    <div className="create-room-page">
      {connectedToRoomServer && (
    <div style={{
      padding: "10px",
      background: "lightgreen",
      marginBottom: "10px",
      borderRadius: "8px",
      fontWeight: "bold",
    }}>
      ✅ Connected to room server on port {roomServerPort}!
    </div>
  )}

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
        <select value={level} onChange={(e) => setLevel(e.target.value)}>
          <option>Level 1</option>
          <option>Level 2</option>
          <option>Level 3</option>
        </select>
      </div>

      <div className="buttons">
        <button onClick={createRoom}>Create</button>
        <button onClick={onCancel}>Cancel</button>
      </div>
    </div>
  );
}

export default CreateRoomPage;
