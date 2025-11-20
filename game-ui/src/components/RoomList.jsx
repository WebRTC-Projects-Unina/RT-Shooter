import { useEffect, useState } from "react";
import { io } from "socket.io-client";

const socket = io("http://localhost:8080");

function RoomList({ onBack, onCreateRoom }) {

  const [rooms, setRooms] = useState([]);

  useEffect(() => {
    socket.emit("request_lobby_list"); 
    // Quando arriva la lista dal server → aggiornala
    socket.on("lobby_list", (data) => {
      console.log("Lobby list received:", data);
      setRooms(data);
    });

    // cleanup quando il componente si unmounta
    return () => {
      socket.off("lobby_list");
    };
  }, []);

  return (
    <div className="lobby-layout">
      
      <div className="create-room-panel">
        <h3>Room Options</h3>

        <button className="go-create-btn" onClick={onCreateRoom}>
          Create New Room
        </button>
      </div>

      {/* SEZIONE DESTRA: TABELLA DELLE ROOM */}
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
                </span>

                <span className="col-action">
                  <button className="join-btn">Join</button>
                </span>
              </div>
            ))}
          </div>

        </div>

        <button className="back-btn" onClick={onBack}>Back</button>
      </div>

    </div>
  );
}

export default RoomList;
