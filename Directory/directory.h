#pragma once
#include "fat_structs.h"
#include "device.h"


// ============================================================
// directory.h / directory.cpp
// TRÁCH NHIỆM: Dev B
// Chức năng 2: Duyệt đệ quy toàn bộ thư mục, tìm file *.txt.
// ============================================================


// ------------------------------------------------------------
// Liệt kê tất cả file *.txt trên toàn thiết bị (kể cả thư mục con)
//
// handle:   handle trả về từ openDevice()
// boot:     struct BootSector đã parse
// fatTable: bảng FAT đã nạp vào bộ nhớ
// result:   vector sẽ chứa thông tin các file .txt tìm được
//
// Hàm này là entry point cho Chức năng 2.
// Nội bộ nó gọi scanDirectory() bắt đầu từ rootCluster.
// GUI gọi hàm này sau khi mở thiết bị và nạp FAT.
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
//
// Hàm này gọi đệ quy chính mình khi gặp thư mục con.
// Bỏ qua entry "." và ".." để tránh đệ quy vô tận.
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
//
// Mỗi entry thư mục có kích thước cố định 32 byte.
// Một cluster chứa (bytesPerSector * sectorsPerCluster / 32) entry.
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
//
// Hàm này không phân biệt file hay thư mục - caller tự kiểm tra
// thuộc tính ATTR_DIRECTORY trong out.attributes sau khi gọi.
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
