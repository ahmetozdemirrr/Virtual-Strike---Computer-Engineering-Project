import WebSocket, { WebSocketServer } from 'ws';
import { randomUUID } from 'crypto';

const clients = new Map();
const wss = new WebSocketServer({ port: 3030 });

wss.on('connection', (ws) => {
    const id = randomUUID();
    clients.set(ws, id);
    console.log(`new connection assigned id: ${id}`);
    
    ws.on('message', (data, isBinary) => {
        if (isBinary) {
            serverBroadcast(data, isBinary);
        } else {
            console.log(`Text message received: ${data}`);
        }
    });

    ws.on('close', () => {
        console.log(`connection (id = ${clients.get(ws)}) closed`);
        clients.delete(ws);
    });

    ws.on('error', (error) => {
        console.error('WebSocket error:', error);
    });
});

function serverBroadcast(message, isBinary) {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message, { binary: isBinary });
        }
    });
}

console.log('The server is running and waiting for connections: 3030');
