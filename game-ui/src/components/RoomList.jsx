import { useEffect, useState } from "react";
import PasswordModal from "./PasswordModal";
import "../styles/roomList.css";
import socket from "../DispatcherSocket.js"


function RoomList({ nickname, onBack, onCreateRoom, setScreen, setGameInfo}) {
  const [rooms, setRooms] = useState([]);
  const [askingPassword, setAskingPassword] = useState(null);
  const [wrongPassword, setWrongPassword] = useState(false); 

  useEffect(() => {
    // Chiedo lista delle lobby
    socket.emit("request_lobby_list");

    // Ricevo lista lobby
    socket.on("lobby_list", (data) => {
      console.log("Lobby list received:", data);
      setRooms(data);
    });

    // JOIN ACCETTATO
    socket.on("join_accepted", ({ roomID, port }) => {
      console.log("Join accepted, connecting to room:", roomID, "port:", port);
    
        // Notifica al dispatcher
        socket.emit("player_joined", {
          roomID,
          nickname
        });

        // PASSA ALLA SCHERMATA DI GIOCO
        setGameInfo({ roomID, port });
        setScreen("game");
    
    });

    // JOIN NEGATO
    socket.on("join_denied", (msg) => {
    if (msg === "Incorrect password") {           
    setWrongPassword(true);                 
    setAskingPassword((prev) => ({ ...prev }));
    return;
  }

    alert(msg); // Per altri tipi di errori NON legati alla password
  });


    return () => {
      socket.off("lobby_list");
      socket.off("join_accepted");
      socket.off("join_denied");
    };
  }, []);

  function handleJoin(room) {
    if (room.hasPassword) {
      setAskingPassword(room);
      return;
    }

    socket.emit("join_room", {
      roomID: room.id,
      nickname,
      password: ""
    });
  }

  return (
    <div className="lobby-layout">
      <div className="lobby-logo-container">
      <img src="/assets/logo.png" alt="RT Shooter Logo" className="lobby-logo" />
      </div>
      <div className="welcome-bar">
        Welcome, <strong>{nickname}</strong>
      </div>

      

      <div className="create-room-panel">
        <button className="go-create-btn" onClick={onCreateRoom}>
          Create New Room
        </button>
      </div>

      <div className="room-list-container">
        <h2>Available Rooms</h2>

        <div className="room-table">
          <div className="room-table-header">
            <span className="col-name">Room Name</span>
            <span className="col-creator">Creator</span>
            <span className="col-status">Status</span>
            <span className="col-action">Action</span>
          </div>

          <div className="room-table-body">
            {rooms.map((room) => (
              <div key={room.id} className="room-row">
                <span className="col-name">{room.name}</span>
                <span className="col-creator">{room.creator}</span>

                <span className="col-status">
                  {room.hasPassword ? "🔒 Private" : "🔓 Open"}
                  {" · "}
                  {(room.playerCount)}/{room.maxPlayers}
                </span>

                <span className="col-action">
                  <button
                    className="join-btn"
                    onClick={() => handleJoin(room)}
                    disabled={room.playerCount >= room.maxPlayers}
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
        incorrect={wrongPassword}               
        onCancel={() => {
          setAskingPassword(null);
          setWrongPassword(false);               
        }}
        onConfirm={(pwd) => {
          setWrongPassword(false);               
          socket.emit("join_room", {
            roomID: askingPassword.id,
            nickname,
            password: pwd
          });
        }}
      />
    )}

    </div>
  );
}

export default RoomList;
