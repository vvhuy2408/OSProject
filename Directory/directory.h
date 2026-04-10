#pragma once
#include "fat_structs.h"
#include "device.h"


// ============================================================
// directory.h / directory.cpp
// Duyệt đệ quy toàn bộ thư mục, tìm file *.txt.
// ============================================================

static std::string extractLFNChars(const uint8_t* raw);
std::string parseLFN(const std::vector<std::vector<uint8_t>>& lfnEntries);

// ------------------------------------------------------------
// Liệt kê tất cả file *.txt trên toàn thiết bị (kể cả thư mục con)
//
// handle:   handle trả về từ openDevice()
// boot:     struct BootSector đã parse
// fatTable: bảng FAT đã nạp vào bộ nhớ
// result:   vector sẽ chứa thông tin các file .txt tìm được
// ------------------------------------------------------------
void listAllTxtFiles(DeviceHandle handle,
                     const BootSector& boot,
                     const std::vector<uint32_t>& fatTable,
                     std::vector<DirEntry>& result);


// ------------------------------------------------------------
// Duyệt đệ quy một thư mục và tất cả thư mục con của nó
//
// handle:       handle thiết bị
// boot:         Boot Sector
// fatTable:     bảng FAT
// startCluster: cluster đầu tiên của thư mục cần duyệt
// currentPath:  đường dẫn hiện tại (để điền vào DirEntry.fullPath)
//               ví dụ: "/", "/DOCS/", "/DOCS/SCHOOL/"
// result:       danh sách file .txt tìm được (thêm vào, không ghi đè)
// ------------------------------------------------------------
void scanDirectory(DeviceHandle handle,
                   const BootSector& boot,
                   const std::vector<uint32_t>& fatTable,
                   uint32_t startCluster,
                   const std::string& currentPath,
                   std::vector<DirEntry>& result);


// ------------------------------------------------------------
// Đọc tất cả entry trong một cluster thư mục vào buffer
//
// handle:        handle thiết bị
// boot:          Boot Sector
// clusterNum:    cluster cần đọc
// entriesOut:    vector nhận các entry raw (mỗi entry là mảng 32 byte)
//
// Trả về: true nếu đọc thành công
// ------------------------------------------------------------
bool readDirectoryCluster(DeviceHandle handle,
                          const BootSector& boot,
                          uint32_t clusterNum,
                          std::vector<std::vector<uint8_t>>& entriesOut);


// ------------------------------------------------------------
// Parse một entry 32 byte raw thành struct DirEntry
//
// rawEntry:    con trỏ đến 32 byte raw của entry
// currentPath: đường dẫn thư mục chứa entry này
// out:         struct DirEntry sẽ được điền dữ liệu
//
// Trả về: true nếu entry hợp lệ (không phải entry trống hoặc đã xóa)
// ------------------------------------------------------------
bool parseDirectoryEntry(const uint8_t* rawEntry,
                         const std::string& currentPath,
                         DirEntry& out);


// ------------------------------------------------------------
// Kiểm tra một entry có phải là file .txt không
//
// entry: struct DirEntry đã parse
//
// Trả về: true nếu là file thường (không phải thư mục)
//         VÀ phần mở rộng là "TXT"
// ------------------------------------------------------------
bool isTxtFile(const DirEntry& entry);


// ------------------------------------------------------------
// Kiểm tra một entry có phải là thư mục con (không phải . hoặc ..) không
//
// entry: struct DirEntry đã parse
//
// Trả về: true nếu là thư mục và tên không phải "." hoặc ".."
// ------------------------------------------------------------
bool isSubDirectory(const DirEntry& entry);
