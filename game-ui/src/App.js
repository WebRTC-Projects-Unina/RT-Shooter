import { useState } from "react";
import MainMenu from "./components/MainMenu";
import RoomList from "./components/RoomList";
import CreateRoomPage from "./components/CreateRoomPage";
import NicknameModal from "./components/NicknameModal";
import Game from "./components/Game";



function App() {
  const [screen, setScreen] = useState("main-menu");
  const [nickname, setNickname] = useState(null);
  const [gameInfo, setGameInfo] = useState(null);



  return (
    <div className="app">

      {/* MAIN MENU */}
      {screen === "main-menu" && (
        <MainMenu
          onPlay1v1={() => {
          if (!nickname) setScreen("ask-nickname");
          else setScreen("lobby");
          }}
              onSettings={() => setScreen("settings")}
              onCredits={() => setScreen("credits")}
        />
      )}
          {screen === "credits" && (
      <div className="screen credits-screen">
        <h2>Credits</h2>

        <div className="credits-container">
          <div className="credit-card">
            <img src="/assets/creator1.png" alt="Creator 1" />
            <p>
              Francesco Gaetano Niutta <br />
              <a 
                href="https://github.com/fniutta" 
                target="_blank" 
                rel="noopener noreferrer"
              >
                @fniutta
              </a>
            </p>

          </div>

          <div className="credit-card">
            <img src="/assets/creator2.jpg" alt="Creator 2" />
            <p>
              Francesco Prisco <br />
              <a 
                href="https://github.com/Azzerot" 
                target="_blank" 
                rel="noopener noreferrer"
              >
                @Azzerot
              </a>
            </p>
            
          </div>
        </div>

        <button className="back-btn" onClick={() => setScreen("main-menu")}>
          Back
        </button>
      </div>
    )}

      {/* NICKNAME MODAL */}
     {screen === "ask-nickname" && (
    <NicknameModal
      onConfirm={(name) => {
        if (name === null) {
          // Utente ha premuto BACK
          setScreen("main-menu");
          return;
        }

        // Utente ha confermato il nickname
        setNickname(name);
        setScreen("lobby");
      }}
    />
  )}


      {/* LOBBY */}
      {screen === "lobby" && (
        
        <RoomList
          nickname={nickname}
          onBack={() => setScreen("main-menu")}
          onCreateRoom={() => setScreen("create-room")}
          setScreen={setScreen}
          setGameInfo={setGameInfo}
        />
      )}


      {/* SETTINGS */}
      {screen === "settings" && (
        <div className="screen">
          <h2>Settings (to be implemented!)</h2>
          <button onClick={() => setScreen("main-menu")}>Back</button>
        </div>
      )}

      {/* CREATE ROOM PAGE */}
      {screen === "create-room" && (
      <CreateRoomPage
        nickname={nickname}
        onCancel={() => setScreen("lobby")}
        setScreen={setScreen}
        setGameInfo={setGameInfo}
      />
    )}


          {screen === "game" && (
      <Game
        roomID={gameInfo.roomID}
        port={gameInfo.port}
        nickname={nickname}
        setScreen={setScreen}
      />
    )}


    </div>
  );
}

export default App;
