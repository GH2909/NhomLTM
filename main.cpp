#include "Data\StockData.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Test dữ liệu cổ phiếu lần đầu ===\n";
    std::cout << stockData();  // In dữ liệu ban đầu

    std::cout << "\n=== Thêm mã cổ phiếu: VNM ===\n";
    addCK("VNM");               // Thêm một mã mới
    std::cout << stockData();  // In lại dữ liệu sau khi thêm

    std::cout << "\n=== Xóa mã cổ phiếu: SS ===\n";
    removeCK("SS");            // Xóa mã cổ phiếu "SS"
    std::cout << stockData();  // In lại sau khi xóa

    std::cout << "\n=== In dữ liệu cập nhật mỗi 1 phút (2 lần) ===\n";
    for (int i = 0; i < 2; ++i) {
        std::cout << stockData();
        std::this_thread::sleep_for(std::chrono::minutes(1)); // đợi 1 phút
    }

    return 0;
}
