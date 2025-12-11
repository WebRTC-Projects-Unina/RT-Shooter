import { useEffect } from "react";
import { io } from "socket.io-client";
import socket from "../index";

var setupDone = false;

 
// creo la socket
var gameSocket = null;
var callStarted = false;
var playerNicknameToSet = "";  // Salva il nickname da passare al C++
// Configurazione server STUN (necessario per attraversare i NAT/Router)
const rtcConfig = {
    iceServers: [
        { urls: 'stun:stun.l.google.com:19302' }
    ]
};

const remoteAudio = document.createElement("audio");
remoteAudio.setAttribute("id", "remoteAudio");
let localStream;
let peerConnection;
let isCaller = false;


async function startAudio() {
    try {
        localStream = await navigator.mediaDevices.getUserMedia({ audio: {
    autoGainControl: false,
    channelCount: 2,
    echoCancellation: false,
    latency: 0,
    noiseSuppression: false,
    sampleRate: 48000,
    sampleSize: 16,
    volume: 1.0
  }, video: false });
        console.log("Microfono attivo. In attesa di connessione...");
    } catch (err) {
        console.error('Errore accesso microfono:', err);
    }
    
}


function sendSignal(type, data) {
    gameSocket.emit(type, JSON.stringify(data));
}

// 3. Creazione e gestione RTCPeerConnection
function createPeerConnection() {
    peerConnection = new RTCPeerConnection(rtcConfig);

    // Aggiungi le tracce audio locali alla connessione
    localStream.getTracks().forEach(track => {
        peerConnection.addTrack(track, localStream);
    });

    // Quando troviamo un percorso di rete (ICE Candidate), inviamolo all"altro
    peerConnection.onicecandidate = (event) => {
        if (event.candidate) {
            sendSignal("candidate", event.candidate );
        }
    };

    // Quando arriva l'audio remoto
    peerConnection.ontrack = (event) => {
      console.log("Stream remoto ricevuto!");
        remoteAudio.srcObject = event.streams[0];
        // Tentativo esplicito di avviare l'audio
    remoteAudio.play().catch(e => console.error("Errore autoplay:", e));
    };
}



async function startAudioCall(){


    isCaller = true; // Chi clicca il bottone inizia la chiamata (Offer)
    await startAudio();
    createPeerConnection();
    
    // Crea l'offerta SDP
    const offer = await peerConnection.createOffer();
    await peerConnection.setLocalDescription(offer);
    
    // Invia l'offerta via WebSocket
    sendSignal("offer",offer );
    console.log("Chiamata in corso...");
};





function Game({ roomID, port, nickname, setScreen }) {




let script = document.createElement("script");

 if(setupDone) {
    setupDone = false;
    window.Module.callMain([]);
  }
  if(gameSocket === null){
  //gameSocket = io(`http://${window.location.hostname}:${port}`, {
  gameSocket = io(`wss://gameserver${port}.${window.location.hostname}`, {
                  query: { roomID, nickname }
                });

  }

  
  useEffect(() => {
    const canvas = document.getElementById("canvas");

    // creo Module
    window.Module = {
      canvas: canvas,
      locateFile: (file) => "/" + file,
      socket: null,
      RegisterSocketIOCallback: null,
      exitGame: () => {setupDone = true; setScreen("lobby");},
      onRuntimeInitialized: () => {
        window.Module.callMain([]); 
        setupDone = true;
        // Setta il nickname DOPO che il runtime è pronto
        if (playerNicknameToSet) {
          window.Module.ccall('SetPlayerNickname', null, ['string'], [playerNicknameToSet]);
        }
        // Registra le callback socket quando il runtime è pronto
        if (window.Module.RegisterSocketIOCallback) {
          window.Module.RegisterSocketIOCallback();
        }
        // ADESSO che i callback sono registrati, fai join_game
        gameSocket.emit("join_game", { nickname: playerNicknameToSet });
        console.log("join_game emitted AFTER callbacks registered");
      }
    };


    
    gameSocket.on("connect", () => {
      console.log("Connected to room server:", port);
      // Salva il nickname per settarlo quando il runtime è pronto
      playerNicknameToSet = nickname;
      
      // Imposta il socket SUBITO, prima di caricare lo script
      window.Module.socket = gameSocket;
      
      script.src = "/test.js";
      script.async = true;
      

        gameSocket.addEventListener("error", (event) => {
          console.log("WebSocket error: ", event);
        });      

          gameSocket.onclose = (event) => {
            console.log("WebSocket disconnessa.");
            
            // 1. Il codice numerico di stato (es. 1000, 1006)
            console.log("Codice (Code):", event.code);
            
            // 2. Il motivo testuale (se il server lo ha inviato)
            console.log("Motivo (Reason):", event.reason);
            
            // 3. Se la chiusura è stata "pulita" o frutto di un errore
            console.log("Chiusura pulita (WasClean):", event.wasClean);
           };

        gameSocket.on("offer", async (data) => 
        {
          
            let sdp = JSON.parse(data);
            if(!peerConnection) return;
            if (!localStream) {
                // Se riceviamo una chiamata e non abbiamo ancora attivato l'audio
                await startAudio();
            }

          // Il ricevente gestisce l'offerta
          await peerConnection.setRemoteDescription(new RTCSessionDescription(sdp));
          const answer = await peerConnection.createAnswer();
          await peerConnection.setLocalDescription(answer);
          sendSignal("answer", answer);
          console.log("offer ricevuto e andato a buon fine");

          });

      gameSocket.on("answer", async (data) => {
        // Il chiamante riceve la risposta
        console.log("aaaa");
        let sdp = JSON.parse(data);
        
        await peerConnection.setRemoteDescription(new RTCSessionDescription(sdp));
        console.log("answer ricevuto");
      });

      gameSocket.on("candidate", async (data) => {

        let candidate = JSON.parse(data);



        if (!localStream) {
              // Se riceviamo una chiamata e non abbiamo ancora attivato l'audio
              await startAudio();
          }
        // Scambio candidati ICE (percorsi di rete)


        if (candidate && candidate.candidate) {
          try{
            await peerConnection.addIceCandidate(new RTCIceCandidate(candidate));
          }
          catch{}
        }
      });


      gameSocket.on("playerCount", async (playerCount) => {
        
        console.log("playerCount: ", playerCount);
        if(callStarted || playerCount < 2) return;
        callStarted = true;
        await startAudioCall();
      });
        
      gameSocket.on('disconnect', function(e) {
      console.log('Got disconnect! ', e);  });

      gameSocket.on('message', function(e) {
      console.log('Got nsg! ', e);  });

      // Il socket è già stato impostato sopra, prima di caricare lo script
      // join_game verrà fatto in onRuntimeInitialized
      if (window.Module.RegisterSocketIOCallback) {
        window.Module.RegisterSocketIOCallback();
      }

    });
    


    script.onload = () => {
      console.log("WASM caricato");
      // Se sia il socket sia la callback sono pronti, registrala
      if (window.Module.socket && window.Module.RegisterSocketIOCallback) {
        window.Module.RegisterSocketIOCallback();
      }
    };

    document.body.appendChild(script);
    document.body.appendChild(remoteAudio);


    return () => {
      console.log("Cleanup Game.jsx");
      gameSocket.disconnect();
      try{
         window.Module.ccall('exitJSCallback', null, [], []);
      }
      catch(err){
      
         console.error(err); 
      }
      document.body.removeChild(script);
    };

  }, []); 


  return (
    <div className="game-screen">
      <canvas
        id="canvas"
      />
    </div>
  );
}

export default Game;
