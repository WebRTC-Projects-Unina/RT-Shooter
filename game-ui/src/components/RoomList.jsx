import { useEffect, useState } from "react";
import { io } from "socket.io-client";
import PasswordModal from "./PasswordModal";
import "../styles/roomList.css";

const socket = io("http://localhost:8080");

function RoomList({ nickname, onBack, onCreateRoom, setScreen, setGameInfo }) {
  const [rooms, setRooms] = useState([]);
  const [askingPassword, setAskingPassword] = useState(null);
  const [wrongPassword, setWrongPassword] = useState(false); // stato per errore password


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

      const gameSocket = io(`http://localhost:${port}`);

      gameSocket.on("connect", () => {
        console.log("Connected to room server on port", port);

        // Notifica al dispatcher
        socket.emit("player_joined", {
          roomID,
          nickname
        });

        // PASSA ALLA SCHERMATA DI GIOCO
        setGameInfo({ roomID, port });
        setScreen("game");
      });
    });

    // JOIN NEGATO
    socket.on("join_denied", (msg) => {
    if (msg === "Incorrect password") {           // 🆕 controlla risposta
    setWrongPassword(true);                 // attiva errore
    setAskingPassword((prev) => ({ ...prev })); // ri-apre modal
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
      <div className="welcome-bar">
        Welcome, <strong>{nickname}</strong>
      </div>

      <h3>Room Options</h3>

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
        incorrect={wrongPassword}                // 🆕 manda errore al modal
        onCancel={() => {
          setAskingPassword(null);
          setWrongPassword(false);               // 🆕 reset errore
        }}
        onConfirm={(pwd) => {
          setWrongPassword(false);               // 🆕 reset errore ogni tentativo
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
