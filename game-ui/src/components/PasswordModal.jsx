import React, { useState } from "react";
import "../PasswordModal.css";

function PasswordModal({ room, onConfirm, onCancel }) {
  const [password, setPassword] = useState("");

  return (
    <div className="password-backdrop">
      <div className="password-modal">
        <h2>Enter Password</h2>
        <p>Room: <strong>{room.name}</strong></p>

        <input
          type="password"
          placeholder="Room password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
        />

        <div className="pm-buttons">
          <button className="pm-confirm" onClick={() => onConfirm(password)}>
            Join
          </button>
          <button className="pm-cancel" onClick={onCancel}>
            Cancel
          </button>
        </div>
      </div>
    </div>
  );
}

export default PasswordModal;
