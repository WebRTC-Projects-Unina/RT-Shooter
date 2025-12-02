import { useState, useEffect, useRef } from "react";
import "../styles/nickname.css";

function NicknameModal({ onConfirm }) {
  const [nickname, setNickname] = useState("");
  const inputRef = useRef(null);

  useEffect(() => {
    inputRef.current?.focus();
  }, []);

  function handleConfirm() {
    if (nickname.trim() !== "") {
      onConfirm(nickname);
    }
  }

  function handleKeyDown(e) {   
    if (e.key === "Enter") {
      handleConfirm();
    }
  }

  return (
    <div className="modal-overlay">
        <div className="modal-box">
        <h2>Enter your nickname</h2>

        <input
            ref={inputRef}
            type="text"
            placeholder="Your nickname"
            value={nickname}
            onChange={(e) => setNickname(e.target.value)}
            onKeyDown={handleKeyDown}
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
