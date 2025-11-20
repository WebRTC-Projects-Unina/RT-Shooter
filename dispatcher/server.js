const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const path = require("path");

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: "http://localhost:3000",   // la UI in sviluppo
    methods: ["GET", "POST"]
  }
});

// === MEMORIA LOBBY === //
let rooms = [];

// === SERVE UI REACT (build) === //
app.use(express.static(path.join(__dirname, "../game-ui/build")));

app.get("*", (req, res) => {
  res.sendFile(path.join(__dirname, "../game-ui/build", "index.html"));
});

// === SOCKET.IO LOGIC === //
io.on("connection", socket => {
  console.log("New client:", socket.id);

  // appena un client entra → mandagli la lista
  socket.emit("lobby_list", rooms);

  // RICHIESTA LISTA LOBBY
  socket.on("request_lobby_list", () => {
  socket.emit("lobby_list", rooms);
  });

  // CREAZIONE ROOM
  socket.on("create_room", (data) => {
    const newRoom = {
      id: Date.now(),
      name: data.name,
      password: data.password,
      creator: socket.id,
      level: data.level,
      hasPassword: data.password.trim() !== "",
    };

    rooms.push(newRoom);

    // aggiorna TUTTI i client tranne chi ha creato la stanza
    socket.broadcast.emit("lobby_list", rooms);
  });

  socket.on("disconnect", () => {
    console.log("Client disconnected:", socket.id);
  });
});

server.listen(8080, () => {
  console.log("Dispatcher running on http://localhost:8080");
});
