import { useEffect, useState } from "react";
import { io } from "socket.io-client";
import PasswordModal from "./PasswordModal";


const socket = io("http://localhost:8080");

function RoomList({nickname,onBack, onCreateRoom }) {

  const [rooms, setRooms] = useState([]);
  const [askingPassword, setAskingPassword] = useState(null);

  useEffect(() => {
    // Chiedo la lista delle lobby al server
    socket.emit("request_lobby_list"); 

    // 1️⃣ Ricezione lista lobby → aggiornamento
    socket.on("lobby_list", (data) => {
      console.log("Lobby list received:", data);
      setRooms(data);
    });

    // 2️⃣ JOIN ACCETTATO → connettersi al room server
    socket.on("join_accepted", ({ roomID, port }) => {
      console.log("Join accepted, connecting to room:", roomID, "on port:", port);

      const gameSocket = io(`http://localhost:${port}`);

      gameSocket.on("connect", () => {
        console.log("Connected to room server on port", port);

        // comunica al dispatcher che sei ufficialmente dentro la room
        socket.emit("player_joined", {
          roomID,
          nickname: nickname   // <-- assicurati che arrivi dal parent
        });

        // se vuoi passare a GamePage adesso:
        // onJoinSuccess(roomID, port);
      });
    });

    // 3️⃣ JOIN NEGATO (password sbagliata o room piena)
    socket.on("join_denied", (msg) => {
      alert(msg);
    });

    // cleanup quando smonta
    return () => {
      socket.off("lobby_list");
      socket.off("join_accepted");
      socket.off("join_denied");
    };
}, []);


  function handleJoin(room) {
  if (room.hasPassword) {
    // invece di prompt → apri la modale
    setAskingPassword(room);
    return;
  }

  // stanza senza password → join diretto
  socket.emit("join_room", {
    roomID: room.id,
    nickname,
    password: ""
  });
}


  return (
    <div className="lobby-layout">
      
      <div className="welcome-bar">
      <span>Welcome, <strong>{nickname}</strong></span>
      </div>

      <h3>Room Options</h3>

      <div className="create-room-panel">
        <button className="go-create-btn" onClick={onCreateRoom}>
          Create New Room
        </button>
      </div>


      {/*TABELLA DELLE ROOM */}
      <div className="room-list-container">
        <h2>Available Rooms</h2>

        <div className="room-table">
          
          {/* HEADER */}
          <div className="room-table-header">
            <span className="col-name">Room Name</span>
            <span className="col-creator">Creator</span>
            <span className="col-status">Status</span>
            <span className="col-action">Action</span>
          </div>

          {/* BODY */}
          <div className="room-table-body">
            {rooms.map((room) => (
              <div key={room.id} className="room-row">
                <span className="col-name">{room.name}</span>
                <span className="col-creator">{room.creator}</span>

                <span className="col-status">
                  {room.hasPassword ? "🔒 Private" : "🔓 Open"}
                  {" · "}
                  {(room.players?.length || 0)}/{room.maxPlayers}
                </span>

                <span className="col-action">
                 <button
                  className="join-btn"
                  onClick={() => handleJoin(room)}
                  disabled={room.players.length >= room.maxPlayers}
                >
                  Join
                </button>

                </span>
              </div>
            ))}
          </div>

        </div>

        <button className="back-btn" onClick={onBack}>Back</button>
      </div>
    {askingPassword && (
    <PasswordModal
      room={askingPassword}
      onCancel={() => setAskingPassword(null)}
      onConfirm={(pwd) => {
        socket.emit("join_room", {
          roomID: askingPassword.id,
          nickname,
          password: pwd
        });
        setAskingPassword(null);
      }}
    />
  )}
          
    </div>
  );
}

export default RoomList;
