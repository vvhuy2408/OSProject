#include <cstdio>

#include "fat_structs.h"
#include "device.h"
#include "boot_sector.h"
#include "fat_table.h"
#include "directory.h"
#include "file_reader.h"
#include "Scheduler/parser.h"
// #include "scheduler.h"
#include "Scheduler/model.h"
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

    // 4. Load bang FAT
    printf("\n=== Load bang FAT ===\n");
    std::vector<uint32_t> fatTable;
    if (!loadFATTable(handle, boot, fatTable)) {
        printf("THAT BAI: Khong load duoc bang FAT.\n");
        closeDevice(handle);
        return 1;
    }
    printf("OK: Load bang FAT thanh cong.\n");
    printf("  -> So entry: %zu\n", fatTable.size());
    printf("  -> Kich thuoc bang FAT: %u bytes\n", boot.fatSize32 * boot.bytesPerSector);

    // 5. Kiem tra cluster chain cua Root Directory
    // Root Directory bat dau tai rootCluster (thuong la 2)
    // Theo doi chuoi cluster va dem xem co bao nhieu cluster
    printf("\n=== Kiem tra cluster chain cua Root Directory ===\n");
    uint32_t cluster = boot.rootCluster;
    int chainLength = 0;
    while (cluster >= 2 && cluster < 0x0FFFFFF8) {
        printf("  Cluster %u -> ", cluster);
        uint32_t next = getNextCluster(fatTable, cluster);
        printf("%u\n", next);
        cluster = next;
        chainLength++;
        if (chainLength > 100) {
            printf("  ... (dung lai sau 100 buoc, co the bi loop)\n");
            break;
        }
    }
    printf("OK: Root Directory chiem %d cluster.\n", chainLength);

    // 6. In 16 entry dau cua bang FAT de kiem tra
    // Entry 0 va 1 la gia tri dac biet, entry 2 tro di moi la cluster du lieu
    printf("\n=== 16 entry dau cua bang FAT ===\n");
    int limit = (fatTable.size() < 16) ? (int)fatTable.size() : 16;
    for (int i = 0; i < limit; i++) {
        printf("  FAT[%2d] = 0x%08X", i, fatTable[i]);
        uint32_t val = fatTable[i] & 0x0FFFFFFF;
        if (i == 0 || i == 1)    printf("  (reserved)");
        else if (val == 0x0FFFFFFF) printf("  (EOC - cluster cuoi chuoi)");
        else if (val == 0)          printf("  (cluster trong)");
        else                        printf("  (next cluster: %u)", val);
        printf("\n");
    }

    printf("\n========================================\n");
    printf("     TEST GIAI DOAN 2 - DEV B\n");
    printf("========================================\n");

    // --- CHUC NANG 2: Liet ke toan bo file .txt ---
    printf("\n=== [CF2] Liet ke file .txt ===\n");

    std::vector<DirEntry> txtFiles;
    listAllTxtFiles(handle, boot, fatTable, txtFiles);

    if (txtFiles.empty()) {
        printf("Khong tim thay file .txt nao.\n");
        printf("  -> Hay copy it nhat 1 file .txt vao USB roi chay lai.\n");
        printf("  -> Kiem tra them: file co duoi .txt viet THUONG khong?\n");
        printf("     FAT32 luu phan mo rong dang VIET HOA (TXT), neu\n");
        printf("     isTxtFile dung strcmp voi 'TXT' thi file .txt\n");
        printf("     viet thuong tren Linux se khong tim thay duoc.\n");
    } else {
        printf("Tim thay %zu file .txt:\n", txtFiles.size());
        for (size_t i = 0; i < txtFiles.size(); i++) {
            printf("  [%zu] %s\n",       i + 1, txtFiles[i].fullPath.c_str());
            printf("       Kich thuoc : %u bytes\n", txtFiles[i].fileSize);
            printf("       Cluster dau: %u\n",       txtFiles[i].firstCluster);
        }
    }

    // --- CHUC NANG 3: Hien thi thong tin chi tiet file dau tien ---
    if (!txtFiles.empty()) {
        printf("\n=== [CF3] Thong tin chi tiet file: %s ===\n",
            txtFiles[0].fullPath.c_str());

        FileInfo info;
        buildFileInfo(txtFiles[0], info);

        printf("  Ten file    : %s\n",    info.name.c_str());
        printf("  Duong dan   : %s\n",    info.fullPath.c_str());
        printf("  Kich thuoc  : %u bytes\n", info.fileSize);
        printf("  Ngay tao    : %04d/%02d/%02d\n",
            info.creationYear, info.creationMonth, info.creationDay);
        printf("  Gio tao     : %02d:%02d:%02d\n",
            info.creationHour, info.creationMinute, info.creationSecond);

        // Kiem tra nhanh ngay gio co hop ly khong
        bool dateOk = (info.creationYear >= 1980 && info.creationYear <= 2107)
                && (info.creationMonth >= 1   && info.creationMonth <= 12)
                && (info.creationDay   >= 1   && info.creationDay   <= 31);
        bool timeOk = (info.creationHour   <= 23)
                && (info.creationMinute <= 59)
                && (info.creationSecond <= 58);

        if (!dateOk)
            printf("  CANH BAO: Ngay tao vo ly, kiem tra lai offset 22/24"
                " trong parseDirectoryEntry.\n");
        if (!timeOk)
            printf("  CANH BAO: Gio tao vo ly, kiem tra lai offset 22/24"
                " trong parseDirectoryEntry.\n");
        if (dateOk && timeOk)
            printf("  OK: Ngay gio hop le.\n");

        // --- Doc noi dung file ---
        printf("\n=== [CF3] Noi dung file: %s ===\n",
            txtFiles[0].fullPath.c_str());

        std::string content;
        bool readOk = readFileContent(handle, boot, fatTable,
                                    txtFiles[0].firstCluster,
                                    txtFiles[0].fileSize,
                                    content);
        if (!readOk) {
            printf("THAT BAI: Khong doc duoc noi dung file.\n");
            printf("  -> Kiem tra lai readSectors va clusterToSector.\n");
        } else {
            printf("Doc thanh cong %zu bytes (fileSize khai bao: %u bytes).\n",
                content.size(), txtFiles[0].fileSize);
            printf("--- Noi dung ---\n%s\n", content.c_str());

            // --- Parse danh sach tien trinh ---
            printf("\n=== [CF3] Parse danh sach tien trinh ===\n");
            std::vector<SchedulingQueue> qList;
            std::vector<Process> pList;

            // parse input file
            Parser parser;
            parser.parseFromString(content, qList, pList);

            if (pList.empty()) {
                printf("Khong parse duoc tien trinh nao.\n");
                printf("  -> Kiem tra dinh dang file: dong dau co phai header khong,\n");
                printf("     cac gia tri cach nhau bang dau phay chua?\n");
            } else {
                printf("Parse thanh cong %zu tien trinh:\n", pList.size());
                printf("  %-6s %-10s %-8s %-10s\n",
                    "PID", "Arrival", "Burst", "Queue");
                printf("  %s\n", std::string(46, '-').c_str());
                for (auto& p : pList) {
                    printf("  %-6s %-10d %-8d %-10d \n",
                        p.pID.c_str(), p.arrivalTime, p.burstTime,
                        p.curQueueID);
                }
            }
        }

        // --- Neu co nhieu hon 1 file, in ten cac file con lai ---
        if (txtFiles.size() > 1) {
            printf("\n=== Cac file .txt con lai (chua doc noi dung) ===\n");
            for (size_t i = 1; i < txtFiles.size(); i++) {
                FileInfo fi;
                buildFileInfo(txtFiles[i], fi);
                printf("  [%zu] %s | %u bytes | %04d/%02d/%02d %02d:%02d:%02d\n",
                    i + 1,
                    fi.fullPath.c_str(), fi.fileSize,
                    fi.creationYear, fi.creationMonth, fi.creationDay,
                    fi.creationHour, fi.creationMinute, fi.creationSecond);
            }
        }
    }

    // 4. Đóng thiết bị
    closeDevice(handle);

    printf("\n=== DONE ===\n");
    return 0;
}

// terminal: g++ main.cpp Device/device.cpp Bootsector/boot_sector.cpp -I./ -I./Bootsector -I./Device -o fat32_reader
// cmd (run as admin): cd <folder chứa main.cpp>
// cmd (run as admin): fat32_reader.exe \\.\E:   (thay E bằng ổ USB của bạn)