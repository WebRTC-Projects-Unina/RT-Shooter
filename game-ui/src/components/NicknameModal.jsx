import { useState } from "react";
import "../nickname.css";

function NicknameModal({ onConfirm }) {
  const [nickname, setNickname] = useState("");

  function handleConfirm() {
    if (nickname.trim() !== "") {
      onConfirm(nickname);
    }
  }

    return (
    <div className="modal-overlay">
        <div className="modal-box">
        <h2>Enter your nickname</h2>

        <input
            type="text"
            placeholder="Your nickname"
            value={nickname}
            onChange={(e) => setNickname(e.target.value)}
        />

        <button className="confirm-btn" onClick={handleConfirm}>
            Confirm
        </button>

        <button className="back-btn" onClick={() => onConfirm(null)}>
            Back
        </button>
        </div>
    </div>
    );
}

export default NicknameModal;
