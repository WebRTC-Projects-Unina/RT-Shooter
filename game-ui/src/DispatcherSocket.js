import { io } from "socket.io-client";
//const socket = io(`http://${window.location.hostname}:8080`);
const socket = io(`wss://dispatcher.${window.location.hostname}`);

export default socket