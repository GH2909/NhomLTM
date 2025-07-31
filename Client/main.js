const boardSize = 15;
let currentPlayer = "X";

function initBoard() {
    const board = document.getElementById("board");
    board.innerHTML = "";
    for (let i = 0; i < boardSize; i++) {
        for (let j = 0; j < boardSize; j++) {
            const cell = document.createElement("div");
            cell.className = "cell";
            cell.dataset.row = i;
            cell.dataset.col = j;
            cell.addEventListener("click", handleClick);
            board.appendChild(cell);
        }
    }
}

function handleClick(e) {
    const row = parseInt(e.target.dataset.row);
    const col = parseInt(e.target.dataset.col);
    sendMove(row, col); // Gửi lên server
}

function updateBoard(newBoard) {
    const cells = document.querySelectorAll(".cell");
    for (let cell of cells) {
        const row = parseInt(cell.dataset.row);
        const col = parseInt(cell.dataset.col);
        const symbol = newBoard[row][col];
        cell.textContent = symbol || "";
    }
}

function updateStatus(text) {
    document.getElementById("status").textContent = text;
}

window.onload = () => {
    initBoard();
};
