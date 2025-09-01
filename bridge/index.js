const WebSocket = require("ws");
const net = require("net");

const wss = new WebSocket.Server({ port: 3000 });
let cppClient = null;
let webClients = [];

function connectCppServer() {
  cppClient = net.createConnection({ port: 8080 }, () => {
    console.log("Connected to C++ server");
  });

  cppClient.on("data", (data) => {
    // broadcast cho tất cả web client
    webClients.forEach((ws) => {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(data.toString());
      }
    });
  });

  cppClient.on("end", () => {
    console.log("C++ server disconnected. Reconnecting...");
    setTimeout(connectCppServer, 5000);
  });

  cppClient.on("error", () => {
    console.log("Error connecting to C++ server. Retry...");
    setTimeout(connectCppServer, 5000);
  });
}

connectCppServer();

wss.on("connection", (ws) => {
  console.log("Web client connected");
  webClients.push(ws);

  ws.on("message", (msg) => {
    // Forward message tới C++ server
    if (cppClient && cppClient.writable) cppClient.write(msg.toString());
  });

  ws.on("close", () => {
    webClients = webClients.filter((c) => c !== ws);
  });
});

console.log("Bridge running at ws://localhost:3000");
