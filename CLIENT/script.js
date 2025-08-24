// Dữ liệu mẫu chứng khoán
let stocks = [
  { code: "VNM", name: "Vinamilk", price: 72000, change: +1.2 },
  { code: "FPT", name: "FPT Corp", price: 95000, change: -0.8 },
  { code: "VCB", name: "Vietcombank", price: 88000, change: +0.5 }
];

// ================= LOGIN ==================
if (document.getElementById("loginForm")) {
  document.getElementById("loginForm").addEventListener("submit", e => {
    e.preventDefault();
    let user = document.getElementById("username").value;
    let pass = document.getElementById("password").value;

    if (user === "admin" && pass === "123") {
      alert("Đăng nhập thành công!");
      window.location.href = "stocks.html";
    } else {
      alert("Sai tài khoản hoặc mật khẩu!");
    }
  });
}

// ================= STOCK LIST ==================
function renderStocks() {
  let tbody = document.querySelector("#stockTable tbody");
  if (!tbody) return;

  tbody.innerHTML = "";
  stocks.forEach(s => {
    let row = document.createElement("tr");
    row.innerHTML = `
      <td>${s.code}</td>
      <td>${s.name}</td>
      <td>${s.price.toLocaleString()} đ</td>
      <td class="${s.change >= 0 ? "positive" : "negative"}">${s.change}%</td>
    `;
    tbody.appendChild(row);
  });
}
renderStocks();

// Search
let searchInput = document.getElementById("search");
if (searchInput) {
  searchInput.addEventListener("input", function () {
    let keyword = this.value.toUpperCase();
    document.querySelectorAll("#stockTable tbody tr").forEach(row => {
      let code = row.cells[0].innerText.toUpperCase();
      row.style.display = code.includes(keyword) ? "" : "none";
    });
  });
}

// ================= CREATE COMPANY ==================
if (document.getElementById("createForm")) {
  document.getElementById("createForm").addEventListener("submit", e => {
    e.preventDefault();
    let code = document.getElementById("code").value.toUpperCase();
    let name = document.getElementById("name").value;
    let price = Number(document.getElementById("price").value);

    if (stocks.some(s => s.code === code)) {
      alert("Mã công ty đã tồn tại!");
      return;
    }

    stocks.push({ code, name, price, change: 0 });
    alert("Thêm công ty thành công!");
    window.location.href = "stocks.html";
  });
}
