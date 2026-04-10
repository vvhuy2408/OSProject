#pragma once
#include "fat_structs.h"
#include "device.h"


// ============================================================
// file_reader.h / file_reader.cpp
// Đọc nội dung file theo cluster chain,
// parse thành FileInfo và danh sách Process.
// ============================================================


// ------------------------------------------------------------
// Đọc toàn bộ nội dung file vào chuỗi text
//
// handle:       handle thiết bị
// boot:         Boot Sector
// fatTable:     bảng FAT đã nạp
// firstCluster: cluster đầu tiên của file (lấy từ DirEntry.firstCluster)
// fileSize:     kích thước thật của file tính bằng byte (lấy từ DirEntry.fileSize)
// contentOut:   chuỗi sẽ chứa nội dung file sau khi đọc
//
// Trả về: true nếu đọc thành công
// ------------------------------------------------------------
bool readFileContent(DeviceHandle handle,
                     const BootSector& boot,
                     const std::vector<uint32_t>& fatTable,
                     uint32_t firstCluster,
                     uint32_t fileSize,
                     std::string& contentOut);


// ------------------------------------------------------------
// Tạo struct FileInfo từ một DirEntry (Chức năng 3 - phần thông tin file)
//
// entry:   DirEntry đã parse từ directory.cpp
// out:     struct FileInfo sẽ được điền dữ liệu
//
// Hàm này decode ngày giờ từ dạng packed bit FAT32 sang ngày giờ thông thường.
// Công thức decode:
//   year   = ((date >> 9) & 0x7F) + 1980
//   month  = (date >> 5) & 0x0F
//   day    =  date & 0x1F
//   hour   = (time >> 11) & 0x1F
//   minute = (time >> 5)  & 0x3F
//   second = (time & 0x1F) * 2       <- FAT lưu giây / 2
// ------------------------------------------------------------
void buildFileInfo(const DirEntry& entry, FileInfo& out);
