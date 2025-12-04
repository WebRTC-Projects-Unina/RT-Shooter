# RT-Shooter - Real-Time 1v1 Shooter Game



---

<p align="center">
  <img src="./game-ui/public/assets/Logo.png" width="450px">
</p>


RT-Shooter è un gioco FPS multiplayer 1v1 che può essere giocato direttamente nel browser. La game UI (Frontend) è sviluppata con **React** ed il dispatcher (backend) con **Node.js**. Inoltre, il gioco utilizza **WebRTC** per la comunicazione in tempo reale, Socket.IO per gestire la sincronizzazione tra i client (passando sempre per il server), e **WebAssembly** per un motore di gioco reattivo e performante.

---
1. [**Caratteristiche Principali**](#caratteristiche-principali)
2. [**Tecnologie Utilizzate**](#tech-stack)
3. [**Setup del Progetto**](#setup-del-progetto)
---
## Caratteristiche principali

- Chiamata Vocale peer-to-peer per interazione tra i giocatori con **WebRTC**
- Sincronizzazione dei movimenti, degli eventi tra i giocatori e della chat testuale via **Socket.IO**
- Motore di gioco sviluppato in **C++** e compilato in **WebAssembly** con **Emscripten**
- Interfaccia moderna e responsive con **React**
- creazione on–demand di stanze dedicate attraverso il dispatcher (**Node.js**)

---

## Interfaccia dell'app

### Schermata principale

<div style="display: flex; gap: 10px;">
  <img src="./game-ui/public/assets/schermata1.jpg" alt="Home screen 1" width="45%"/>
  <img src="./game-ui/public/assets/schermata2.png" alt="Home screen 2" width="45%"/>
</div>

---

## Modalità di gioco

### Sparatutto 1v1


![Shooter](./assets/sparatutto.png)

---

## Tech Stack

- **Frontend:** React + WebRTC
- **Backend:** Node.js + Socket.IO
- **Motore di gioco:** **C++** compilato in WebAssembly (**WASM**)
- **Styling:** CSS


---

# Setup del progetto

Per avviare il progetto, segui questi passaggi:

### 1. **Clone della repository**
```
git clone https://github.com/WebRTC-Projects-Unina/RT-Shooter
```
### 2. **Installare Node.js**
 
Il progetto richiede **Node.js** (versione 18 o superiore) per gestire le dipendenze. Puoi scaricare e installare Node.js dal [sito ufficiale](https://nodejs.org/).
 
Verifica di avere la versione corretta di Node.js eseguendo:
 
```bash
node -v
```
## 3. **Installare le Dipendenze (Frontend)**
 
Accedi alla cartella  game-ui e installa le dipendenze necessarie utilizzando **npm**
```
cd RT-Shooter/game-ui
npm install
```
## 4. **Installare le Dipendenze (Backend)** 
Accedi alla cartella dispatcher e installa le dipendenze necessarie utilizzando **npm**
```
cd ../dispatcher
npm install
```
## 5. **Fai partire il dispacher (server node)**
Il dispatcher parte sulla porta 8080
```
node .\server.js
```
## 6. **Fai partire l'app React**
App React parte sulla porta 3000
```
cd ../game-ui
npm start
```

# Game engine

I file data.js e data.wasm, presenti nella cartella public del frontend, sono stati generati utilizzando **Emscripten** a partire da un codice sorgente scritto in **C++**, che si trova nella cartella game.

Anche se per il corretto funzionamento del gioco sono necessari solo i file .data .js e .wasm nella cartella public dentro game-ui, è importante fare riferimento al codice sorgente **C++** per completezza. La cartella game contiene il motore di gioco, che è stato sviluppato in **C++** e successivamente compilato in WebAssembly (**WASM**) tramite **Emscripten** per essere eseguito direttamente nel browser.

Questo approccio ci permette di mostrare come il gioco viene implementato inizialmente in **C++** prima di essere compilato in **WASM**, anche se nella versione finale del gioco sono richiesti solo i file generati ( .data .wasm e .js).

Se si volesse modificare la logica di gioco o qualsiasi pezzo di codice inerente al game engine è necessario effettuare le modifiche e poi ricompilare il tutto con **Emscripten**.

## Installazione Emscripten

Fermo restando che tutte le specifiche di **Emscripten** possono essere trovate nella seguente repository Github: https://github.com/Emscripten-core/emsdk.git
Ecco 4 semplici step per installarlo:

### 1. **Clone della repo**
```
git clone https://github.com/Emscripten-core/emsdk.git
```
### 2. **installa l'ultima versione**
```
cd emsdk
./emsdk install latest
```

### 3. **Attiva il tutto**
NOTA BENE: il flag **--permanent** fa si che **Emscripten** attivi la versione richiesta in modo permanente.

Questo significa che l'ambiente di **Emscripten** verrà automaticamente caricato ogni volta che apri una nuova finestra del terminale. 
Senza il flag dovrei ogni volta in volta eseguire eseguire manualmente source ./emsdk_env.sh.
A voi la scelta !
```
./emsdk activate latest --permanent
```
### 4. **Verifica Installazione**
Per verificare la corretta installazione usare il seguente comando:
```
emcc -v
```

## Comando per la compilazione con Emscripten

Il comando è presente interamente in **RT-Shooter/game/README.md** 
I nuovi file generati ( .data .wasm e .js) sovrascriveranno automaticamente i precedenti.
Per comodità è stato scritto un Makefile.

Per compilare entrare, quindi, nella cartella game ed eseguire il comado make.
Ecco la procedura:
```
cd RT-Shooter/game
make
```

