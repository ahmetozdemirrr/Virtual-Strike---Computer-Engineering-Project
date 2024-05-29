import WebSocket, { WebSocketServer } from 'ws';
import { randomUUID } from 'crypto';

const ports = [3030, 3000, 8080];
const servers = [];
const clientsMap = new Map();

// Bu fonksiyon her port için ayrı bir WebSocket sunucusu oluşturur
function createServer(port) 
{
    const clients = new Map();
    clientsMap.set(port, clients);

    const wss = new WebSocketServer({ port });

    wss.on('connection', (ws) => 
    {
        const id = randomUUID();
        clients.set(ws, id);
        console.log(`New connection assigned id: ${id} on port: ${port}`);

        ws.on('message', (data, isBinary) => 
        {
            if (isBinary) 
            {
                //console.log(`Binary message received on port ${port}`);
            }
            
            else 
            {
                //console.log(`Text message received on port ${port}: ${data}`);
            }
            serverBroadcast(port, data, isBinary);
        });

        ws.on('close', () => 
        {
            console.log(`Connection (id = ${clients.get(ws)}) closed on port: ${port}`);
            clients.delete(ws);
        });

        ws.on('error', (error) => 
        {
            console.error('WebSocket error:', error);
        });
    });
    servers.push(wss);
}

// Bu fonksiyon belirli bir port üzerinden tüm bağlantılara mesaj yayını yapar
function serverBroadcast(port, message, isBinary) 
{
    const wss = servers.find(server => server.options.port === port);
    
    if (wss) 
    {
        wss.clients.forEach((client) => 
        {
            if (client.readyState === WebSocket.OPEN) 
            {
                client.send(message, { binary: isBinary });
            }
        });
    }
}

// Belirtilen portlar için sunucuları oluşturur
ports.forEach(createServer);

console.log('The servers are running and waiting for connections on ports: ', ports.join(', '));
