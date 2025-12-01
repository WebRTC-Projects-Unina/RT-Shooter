import React, { useState, useEffect, useRef } from "react";
import "../styles/passwordModal.css";

function PasswordModal({ room, onConfirm, onCancel, incorrect}) {
  const [password, setPassword] = useState("");
  const [error, setError] = useState(""); //  stato per eventuali messaggi di errore

  const passRef = useRef(null);          //  riferimento per l’input

  useEffect(() => {
    passRef.current?.focus();            //  autofocus al caricamento
  }, []);

  useEffect(() => {
  if (incorrect) {
    setError("Incorrect password");
    setPassword("");
    passRef.current?.focus();
    }
  }, [incorrect]);


  function handleKeyDown(e) {            //  gestione ENTER
    if (e.key === "Enter") {
      onConfirm(password);
    }
  }
  return (
    <div className="password-backdrop">
      <div className="password-modal">
        <h2>Enter Password</h2>
        <p>Room: <strong>{room.name}</strong></p>

        <input
          ref={passRef}                      //  assegnazione del riferimento
          type="password"
          placeholder="Room password"
          value={password}
          onChange={(e) => {
            setPassword(e.target.value);
            setError(""); //  reset del messaggio di errore al cambiamento
          }}
          onKeyDown={handleKeyDown}        //  gestione ENTER
        />

         {error && (
          <div className="pm-error" onClick={() => setError("")}>
            {error}
          </div>
        )}

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
