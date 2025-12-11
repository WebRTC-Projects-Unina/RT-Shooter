import { io } from "socket.io-client";
const socket = io(`http://${window.location.hostname}:8080`);

export default socket