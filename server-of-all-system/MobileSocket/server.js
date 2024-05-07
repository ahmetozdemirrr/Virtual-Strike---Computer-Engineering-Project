import WebSocket, { WebSocketServer } from 'ws';
import { randomUUID } from 'crypto';
const clients = new Map(); 
const wss = new WebSocketServer({ port: 3030 }); 
wss.on('connection', (ws) => {
    // create an id to track the client
    const id = randomUUID();
    clients.set(ws, id);
    console.log(`new connection assigned id: ${id}`);
    ws.on('message', (data) => {
        serverBroadcast(`${data}`);
    });
    ws.on('close', () => {
        console.log(`connection (id = ${clients.get(ws)}) closed`);
        clients.delete(ws);
    });
    ws.on('error', (error) => {
        console.error('WebSocket error:', error);
    });
});
function serverBroadcast(message) {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message);
        }
    });
}
console.log('The server is running and waiting for connections: 3030');