import { useState } from "react";

function RoomList({ onBack, onCreateRoom }) {
  // Mock stanze
  const [rooms] = useState([
    { id: 1, name: "Arena #1", creator: "PlayerA", hasPassword: false },
    { id: 2, name: "Training Room", creator: "Marco", hasPassword: true },
    { id: 3, name: "Battle Zone", creator: "Kira", hasPassword: false },
  ]);

 

  

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

              {/* LUCCHEKTTI ALLINEATI */}
              <span className="col-status">
                {room.hasPassword ? "🔒 Private" : "🔓 Open"}
              </span>

              {/* JOIN BUTTON SEMPRE ALLINEATO A DESTRA */}
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
