function MainMenu({ onPlay1v1, onLevelSelect, onSettings }) {
  return (
    <div className="main-menu">
      <h1>RT_SHOOTER</h1>

      <div className="menu-buttons">
        <button onClick={onPlay1v1}>Play 1v1</button>
        <button onClick={onLevelSelect}>Level Select</button>
        <button onClick={onSettings}>Settings</button>
        <button disabled>Voice Chat (coming soon)</button>
      </div>
    </div>
  );
}

export default MainMenu;
