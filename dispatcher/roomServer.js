const http = require("http");
const { Server } = require("socket.io");

const port = process.argv[2];
const roomID = process.argv[3];

const server = http.createServer();

const io = new Server(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"]
  }
});
 
let connectedPlayers = 0;
const playersInRoom = new Map();  // Tiene traccia dei player: socket.id -> nickname

process.stdin.on('data', (data) => {
  const msg = JSON.parse(data.toString());
  console.log('JSON dal padre:', msg);

});

io.on("connection", (socket) => {

  console.log(`[ROOM ${roomID}] Player connected:`, socket.id);
  connectedPlayers++;
  io.emit("playerCount", connectedPlayers);
  process.stdout.write(JSON.stringify({ roomID: roomID, playerCount:  connectedPlayers}) + "\n\r");

  console.log(`[ROOM ${roomID}] Connected players: ${connectedPlayers}`);
  
  socket.on("join_game", (player) => {
    console.log(`[ROOM ${roomID}] Player joined:`, player);
    // Salva il nickname del player
    socket.nickname = player.nickname || player.name || "Unknown";
    
    console.log(`[ROOM ${roomID}] Current players in room BEFORE adding new player:`, Array.from(playersInRoom.entries()));
    
    // PRIMA informa il nuovo player di tutti i player già presenti
    for (const [id, nickname] of playersInRoom) {
      socket.emit("enemy_joined", { nickname: nickname });
      console.log(`[ROOM ${roomID}] Sending existing player ${nickname} to ${socket.nickname}`);
    }
    
    // POI aggiungi il nuovo player alla Map
    playersInRoom.set(socket.id, socket.nickname);
    
    // Informa tutti gli altri player che è entrato qualcuno
    socket.broadcast.emit("enemy_joined", {
      nickname: socket.nickname
    });
    console.log(`[ROOM ${roomID}] Broadcasting new player ${socket.nickname} to others`);
    
    io.emit("playerCount", connectedPlayers);
  });

  socket.on("player_shoot", (data) => {
    // Server relay - invia lo sparo all'altro player CON il nickname del shooter
    socket.broadcast.emit("player_shot", {
      ...data,
      shooterNickname: socket.nickname
    });
  });

  socket.on("player_death", (data) => {
    // Server relay - informa l'altro player che questo player è morto
    socket.broadcast.emit("enemy_died", data);
  });

  socket.onclose = (event) => {
  console.log("WebSocket disconnessa.");
  
  // 1. Il codice numerico di stato (es. 1000, 1006)
  console.log("Codice (Code):", event.code);
  
  // 2. Il motivo testuale (se il server lo ha inviato)
  console.log("Motivo (Reason):", event.reason);
  
  // 3. Se la chiusura è stata "pulita" o frutto di un errore
  console.log("Chiusura pulita (WasClean):", event.wasClean);
  };

  socket.on("disconnect", () => {
  console.log(`[ROOM ${roomID}] Player disconnected:`, socket.id);
  
  // Rimuovi dalla mappa dei player
  playersInRoom.delete(socket.id);
  
  connectedPlayers--;
  io.emit("playerCount", connectedPlayers);
  process.stdout.write(JSON.stringify({ roomID: roomID, playerCount:  connectedPlayers}) + "\n\r");

  console.log(`[ROOM ${roomID}] Connected players: ${connectedPlayers}`);

    // SE LA ROOM È VUOTA → CHIUDE IL PROCESSO
    if (connectedPlayers === 0) {
      console.log(`[ROOM ${roomID}] Room empty → shutting down`);
      process.exit(0); // termina il processo del room server
    }
  });

      socket.on("player_update", (data) => {
      //console.log(`[ROOM ${roomID}] Update da ${socket.id}:`, data);
      socket.broadcast.emit("enemy_update", data);
    });
    
    // Gestione messaggi di chat
    socket.on("chat_message", (data) => {
      console.log(`[ROOM ${roomID}] Chat message da ${socket.id}:`, data.message);
      socket.broadcast.emit("chat_message_received", {
        message: data.message,
        senderNickname: socket.nickname || "Unknown"
      });
    });

    socket.on("offer", (message) => {
        console.log(`aaaaaa`);
        socket.broadcast.emit("offer", message);
    });

    socket.on("answer", (message) => {
        console.log(`bb`);
        socket.broadcast.emit("answer", message);
    });
    socket.on("candidate", (data) => {
      try{
        console.log(`cc`);
        socket.broadcast.emit("candidate", data);
        }
        catch(err){
          console.log(err);
        }
    });



});

server.listen(port, () => {
  console.log(`[ROOM ${roomID}] Room server started on port ${port}`);
});
