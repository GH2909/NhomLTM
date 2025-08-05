
const WebSocket = require('ws');
const net = require('net');

const wss = new WebSocket.Server({ port: 8080 });
let players = { X: null, O: null };
let tcpClients = { X: null, O: null };

function createTcpClient(player) {
  const tcpClient = new net.Socket();
  tcpClient.connect(9999, '127.0.0.1', () => {
    console.log(`Da ket noi TCP toi server C++ cho player ${player}`);
  });

  tcpClient.on('data', (data) => {
    // Nhận dữ liệu từ server C++ cho player này, gửi cho WebSocket đối thủ
    const row = data.readUInt8(0);
    const col = data.readUInt8(1);
    const playerFromServer = String.fromCharCode(data[2]);

    const move = JSON.stringify({ row, col, player: playerFromServer });

    // Gửi cho cả người chơi và đối thủ
[players.X, players.O].forEach((client) => {
  if (client && client.readyState === WebSocket.OPEN) {
    client.send(move);
  }
});

    console.log(`Da gui du lieu tu player ${playerFromServer} toi doi thu: (${row}, ${col})`);
  });

  tcpClient.on('close', () => {
    console.log(`TCP client cho player ${player} da ngat ket noi`);
  });

  tcpClient.on('error', (err) => {
    console.log(`Loi TCP client ${player}:`, err.message);
  });


  return tcpClient;
}

wss.on('connection', (ws) => {
  if (!players.X) {
    players.X = ws;
    tcpClients.X = createTcpClient('X');
    ws.player = 'X';
    ws.send(JSON.stringify({ info: "Ban la nguoi choi X" }));
  } else if (!players.O) {
    players.O = ws;
    tcpClients.O = createTcpClient('O');
    ws.player = 'O';
    ws.send(JSON.stringify({ info: "Ban la nguoi choi O" }));
  } else {
    ws.send(JSON.stringify({ error: "Da du 2 nguoi choi" }));
    ws.close();
    return;
  }

  console.log(`Nguoi choi ${ws.player} da ket noi qua WebSocket`);

  ws.on('message', (message) => {
    try {
      const parsed = JSON.parse(message);


    // Nếu là thông báo thắng
      if (parsed.type === "win") {
        const winner = parsed.winner;
        const loser = winner === "X" ? players.O : players.X;


      // Gửi thông báo tới cả hai người chơi
        if (players[winner] && players[winner].readyState === WebSocket.OPEN) {
          players[winner].send(JSON.stringify({ type: "win", winner }));
        }
        if (loser && loser.readyState === WebSocket.OPEN) {
          loser.send(JSON.stringify({ type: "win", winner }));
        }
        return;
    }
      const { row, col, player } = parsed;
      if (player !== ws.player) {
        console.log(`Loi: client gui player khong khop`);
        return;
      }

      const buffer = Buffer.alloc(3);
      buffer.writeUInt8(row, 0);
      buffer.writeUInt8(col, 1);
      buffer.writeUInt8(player.charCodeAt(0), 2);


      // Gửi dữ liệu qua đúng TCP client tương ứng
      if (tcpClients[player]) {
        tcpClients[player].write(buffer);
      }
    } catch (err) {
      console.log("Loi du lieu client gui len:", err);
    }
  });


  ws.on('close', () => {
    console.log(`Nguoi choi ${ws.player} da ngat ket noi qua WebSocket`);
    if (players[ws.player]) {
      players[ws.player] = null;
    }
    if (tcpClients[ws.player]) {
      tcpClients[ws.player].destroy();
      tcpClients[ws.player] = null;
    }
  });
  console.log(" Bridge.js đã khởi chạy, đang lắng nghe WebSocket trên cổng 8080");

});



