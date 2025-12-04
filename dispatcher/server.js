const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const path = require("path");

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: '*',   // la UI in sviluppo
    methods: ["GET", "POST"]
  }
});

const roomProcesses = new Map(); // roomID → child process
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


// === JOIN ROOM (richiesta di entrare in una stanza) ===
socket.on("join_room", ({ roomID, nickname, password }) => {
  const room = rooms.find(r => r.id === roomID);
  if (!room) return;

  // Se la stanza ha password → verifica
  if (room.hasPassword) {
    if (!password || password !== room.password) {
      socket.emit("join_denied", "Incorrect password");
      return;
    }
  }

  // Verifica capienza
  if (room.playerCount >= room.maxPlayers) {
    socket.emit("join_denied", "Room is full");
    return;
  }



  // Mando al client i dati per connettersi al server della room
  socket.emit("join_accepted", {
    roomID,
    port: room.port
  });

  // Aggiorno la lobby a tutti
  io.emit("lobby_list", rooms);
});

  // CREAZIONE ROOM
  socket.on("create_room", (data) => {
    const newRoom = {
      id: Date.now(),
      name: data.name,
      password: data.password,
      creator: data.creator,
      level: data.level,
      hasPassword: data.password.trim() !== "",
      playerCount: 0,        
      maxPlayers: 2,      
      port: null          // da assegnare quando il gioco parte
    };


    // --- assegna porta libera ---
    const port = getFreePort();
    newRoom.port = port;

    // --- avvia room server ---
    startRoomServer(port, newRoom.id);

    // --- rimanda al client info stanza ---
    socket.emit("room_created", { roomID: newRoom.id, port });

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

function getFreePort(start = 9000) {
  const usedPorts = rooms.map(r => r.port).filter(Boolean);
  let port = start;

  while (usedPorts.includes(port)) {
    port++;
  }
  return port;
}

const { spawn } = require("child_process");
let buffer = '';
function startRoomServer(port, roomID) {
  const child = spawn("node", ["roomServer.js", port, roomID], {
    stdio: ['pipe', 'pipe', 'pipe']
  });
  
  child.stdout.on('data', (data) => {
    buffer += data.toString();
    let obj;
    let parts = buffer.split('\n');
    buffer = parts.pop(); // tengo l'ultima riga incompleta

    for (const line of parts) {
        if (!line.trim()) continue; // salta righe vuote
        try {
            const obj = JSON.parse(line);
            const room = rooms.find(r => r.id === Number(obj.roomID));
            if (!room) return;
            room.playerCount = obj.playerCount;

            io.emit("lobby_list", rooms);
            console.log(obj);
        } catch (err) {
            console.log(line);
        }
    }
    
   
  });


  console.log(`Room server for ${roomID} started on port ${port}`);

  // Salva il processo
  roomProcesses.set(roomID, child);

  // Quando il processo muore
  child.on("exit", (code, signal ) => {
    console.log(`Room server for ${roomID} EXITED. Code: ${code}, Signal: ${signal}`);

    // rimuovi la stanza dal dispatcher
    rooms = rooms.filter(r => r.id !== roomID);
    roomProcesses.delete(roomID);

    // aggiorna la lobby a tutti i client
    io.emit("lobby_list", rooms);
  });

  child.on("error", (err) => {
    console.log(`CHILD ERROR: ${err}`);

    // rimuovi la stanza dal dispatcher
    rooms = rooms.filter(r => r.id !== roomID);
    roomProcesses.delete(roomID);

    // aggiorna la lobby a tutti i client
    io.emit("lobby_list", rooms);
  });

  return child;
}


