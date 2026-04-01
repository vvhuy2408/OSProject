#include <cstdio>
// #include <string>
// #include <vector>

#include "fat_structs.h"
#include "device.h"
#include "boot_sector.h"
// #include "fat_table.h"
// #include "directory.h"
// #include "file_reader.h"
// #include "scheduler.h"

// // ============================================================
// // main.cpp
// // Điểm khởi đầu của chương trình.
// // Hiện tại chạy ở chế độ console để test logic trước khi tích hợp GUI.
// // Sau này Dev A và Dev B sẽ thay phần này bằng vòng lặp GUI.
// // ============================================================


// // ------------------------------------------------------------
// // Hàm test tổng hợp chạy toàn bộ luồng từ đầu đến cuối:
// //   Mở thiết bị -> Boot Sector -> FAT Table -> Liệt kê file
// //   -> Chọn file -> Đọc và parse -> Lập lịch -> In kết quả
// //
// // devicePath: ví dụ "\\\\.\\E:" cho ổ E trên Windows
// // ------------------------------------------------------------
// void runConsoleTest(const char* devicePath) {
//     printf("=== FAT32 Reader - Console Test Mode ===\n\n");

//     // --- Bước 1: Mở thiết bị ---
//     printf("[1] Mở thiết bị: %s\n", devicePath);
//     DeviceHandle handle = openDevice(devicePath);
//     if (handle == INVALID_DEVICE_HANDLE) {
//         printf("    THẤT BAI: %s\n", getLastErrorMessage().c_str());
//         printf("    Gợi ý: Chạy chương trình với quyền Administrator.\n");
//         return;
//     }
//     printf("    OK\n\n");

//     // --- Bước 2: Đọc Boot Sector (Dev A) ---
//     printf("[2] Đọc Boot Sector...\n");
//     BootSector boot;
//     if (!readBootSector(handle, &boot)) {
//         printf("    THẤT BAI: Không thể đọc hoặc parse Boot Sector.\n");
//         printf("    Gợi ý: Kiểm tra thiết bị có được format FAT32 không.\n");
//         closeDevice(handle);
//         return;
//     }
//     printBootSector(boot);
//     printf("\n");

//     // --- Bước 3: Nạp bảng FAT (Dev B) ---
//     printf("[3] Nạp bảng FAT...\n");
//     std::vector<uint32_t> fatTable;
//     if (!loadFATTable(handle, boot, fatTable)) {
//         printf("    THẤT BAI: Không thể đọc bảng FAT.\n");
//         closeDevice(handle);
//         return;
//     }
//     printf("    OK - %zu entry\n\n", fatTable.size());

//     // --- Bước 4: Liệt kê file .txt (Dev B) ---
//     printf("[4] Tìm kiếm file *.txt...\n");
//     std::vector<DirEntry> txtFiles;
//     listAllTxtFiles(handle, boot, fatTable, txtFiles);

//     if (txtFiles.empty()) {
//         printf("    Không tìm thấy file .txt nào trên thiết bị.\n");
//         closeDevice(handle);
//         return;
//     }

//     printf("    Tìm thấy %zu file:\n", txtFiles.size());
//     for (size_t i = 0; i < txtFiles.size(); i++) {
//         printf("    [%zu] %s\n", i, txtFiles[i].fullPath.c_str());
//     }
//     printf("\n");

//     // --- Bước 5: Đọc file đầu tiên để test (Dev B) ---
//     printf("[5] Đọc file: %s\n", txtFiles[0].fullPath.c_str());
//     FileInfo fileInfo;
//     std::vector<Process> processes;

//     if (!loadTxtFile(handle, boot, fatTable, txtFiles[0], fileInfo, processes)) {
//         printf("    THẤT BAI: Không thể đọc hoặc parse file.\n");
//         closeDevice(handle);
//         return;
//     }

//     printf("    Tên    : %s\n", fileInfo.name.c_str());
//     printf("    Kích thước: %u bytes\n", fileInfo.fileSize);
//     printf("    Ngày tạo : %04d/%02d/%02d %02d:%02d:%02d\n",
//            fileInfo.creationYear, fileInfo.creationMonth, fileInfo.creationDay,
//            fileInfo.creationHour, fileInfo.creationMinute, fileInfo.creationSecond);
//     printf("    Số tiến trình: %zu\n\n", processes.size());

//     // --- Bước 6: Chạy lập lịch FCFS (Dev A) ---
//     printf("[6] Lập lịch FCFS...\n");
//     ScheduleResult result = runFCFS(processes);

//     printf("    Gantt Chart:\n    ");
//     for (const auto& slot : result.timeline) {
//         printf("[P%d: %d-%d] ", slot.pid, slot.startTime, slot.endTime);
//     }
//     printf("\n\n");

//     printf("    %-5s %-12s %-15s %-12s\n",
//            "PID", "Completion", "Turnaround", "Waiting");
//     for (const auto& r : result.results) {
//         printf("    %-5d %-12d %-15d %-12d\n",
//                r.pid, r.completionTime, r.turnaroundTime, r.waitingTime);
//     }
//     printf("\n    Avg Turnaround: %.2f | Avg Waiting: %.2f\n",
//            result.avgTurnaroundTime, result.avgWaitingTime);

//     // --- Dọn dẹp ---
//     closeDevice(handle);
//     printf("\n=== Test hoàn tất ===\n");
// }


// int main(int argc, char* argv[]) {
//     // Mặc định dùng ổ E, có thể truyền path khác qua argument
//     // Ví dụ chạy: fat32_reader.exe \\.\PhysicalDrive1
//     const char* devicePath = "\\\\.\\E:";
//     if (argc >= 2) {
//         devicePath = argv[1];
//     }

//     runConsoleTest(devicePath);

//     printf("\nBấm Enter để thoát...");
//     getchar();
//     return 0;

//     // TODO: Thay runConsoleTest() bằng khởi tạo GUI khi sẵn sàng
//     // Ví dụ với Qt:   return QApplication(argc, argv) + MainWindow...
//     // Ví dụ với ImGui: return runImGuiApp(argc, argv);
// }

int main(int argc, char* argv[]) {
    const char* devicePath = "\\\\.\\F:";   //check xem cái usb ở ổ nào để thay chữ nha :>
    if (argc >= 2) {
        devicePath = argv[1];
    }

    printf("\n=== TEST BOOT SECTOR ===\n");

    // 1. Mở thiết bị
    DeviceHandle handle = openDevice(devicePath);
    if (handle == INVALID_DEVICE_HANDLE) {
        printf("Failed to open device\n");
        return 1;
    }
    printf("\nOpen device OK\n");

    // 2. Đọc Boot Sector
    BootSector boot;
    if (!readBootSector(handle, &boot)) {
        printf("Failed to read Boot Sector\n");
        closeDevice(handle);
        return 1;
    }

    printf("\nRead Boot Sector OK\n");

    // 3. In thông tin
    printBootSector(boot);

    // 4. Đóng thiết bị
    closeDevice(handle);

    printf("\n=== DONE ===\n");
    return 0;
}

// terminal: g++ main.cpp Device/device.cpp Bootsector/boot_sector.cpp -I./ -I./Bootsector -I./Device -o fat32_reader
// cmd (run as admin): cd <folder chứa main.cpp>
// cmd (run as admin): fat32_reader.exe \\.\E:   (thay E bằng ổ USB của bạn)