const http = require("http");
const { Server } = require("socket.io");

const port = process.argv[2];
const roomID = process.argv[3];

const server = http.createServer();

const io = new Server(server, {
  cors: {
    origin: "http://localhost:3000",
    methods: ["GET", "POST"]
  }
});

let connectedPlayers = 0;

io.on("connection", (socket) => {
  console.log(`[ROOM ${roomID}] Player connected:`, socket.id);
  connectedPlayers++;
  console.log(`[ROOM ${roomID}] Connected players: ${connectedPlayers}`);
  
  socket.on("join_game", (player) => {
    console.log(`[ROOM ${roomID}] Player joined:`, player);
  });

  socket.on("disconnect", () => {
  console.log(`[ROOM ${roomID}] Player disconnected:`, socket.id);
  
  connectedPlayers--;
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


});

server.listen(port, () => {
  console.log(`[ROOM ${roomID}] Room server started on port ${port}`);
});
