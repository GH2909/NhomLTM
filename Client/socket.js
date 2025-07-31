let socket = new WebSocket("ws://localhost:8080");

socket.onopen = () => {
    console.log("✅ WebSocket connected!");
};

socket.onmessage = (event) => {
    const data = JSON.parse(event.data);
    if (data.type === "update") {
        updateBoard(data.board);
        updateStatus(data.status);
    }
};

socket.onerror = (error) => {
    console.error("❌ WebSocket Error:", error);
};

socket.onclose = () => {
    console.warn("🔌 WebSocket connection closed.");
};

function sendMove(row, col) {
    const message = JSON.stringify({ type: "move", row: row, col: col });
    socket.send(message);
}
