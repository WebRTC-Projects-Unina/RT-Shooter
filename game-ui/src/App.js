import { useState } from "react";
import MainMenu from "./components/MainMenu";
import RoomList from "./components/RoomList";
import CreateRoomPage from "./components/CreateRoomPage";

function App() {
  const [screen, setScreen] = useState("main-menu");

  return (
    <div className="app">

      {/* MAIN MENU */}
      {screen === "main-menu" && (
        <MainMenu
          onPlay1v1={() => setScreen("lobby")}
          onLevelSelect={() => setScreen("levels")}
          onSettings={() => setScreen("settings")}
        />
      )}

      {/* LOBBY */}
      {screen === "lobby" && (
        <RoomList
          onBack={() => setScreen("main-menu")}
          onCreateRoom={() => setScreen("create-room")}
        />
      )}

      {/* LEVEL SELECT */}
      {screen === "levels" && (
        <div className="screen">
          <h2>Level Selection (placeholder)</h2>
          <button onClick={() => setScreen("main-menu")}>Back</button>
        </div>
      )}

      {/* SETTINGS */}
      {screen === "settings" && (
        <div className="screen">
          <h2>Settings (placeholder)</h2>
          <button onClick={() => setScreen("main-menu")}>Back</button>
        </div>
      )}

      {/* CREATE ROOM PAGE */}
      {screen === "create-room" && (
        <CreateRoomPage
          onCancel={() => setScreen("lobby")}
          onCreate={(roomData) => {
            console.log("Room Created:", roomData);
            setScreen("lobby"); 
          }}
        />
      )}

    </div>
  );
}

export default App;
