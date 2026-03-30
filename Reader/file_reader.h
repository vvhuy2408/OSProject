// #pragma once
// #include "fat_structs.h"
// #include "device.h"
// #include <vector>
// #include <string>

// // ============================================================
// // file_reader.h / file_reader.cpp
// // TRÁCH NHIỆM: Dev B
// // Chức năng 3 (phần đọc): Đọc nội dung file theo cluster chain,
// // parse thành FileInfo và danh sách Process.
// // ============================================================


// // ------------------------------------------------------------
// // Đọc toàn bộ nội dung file vào chuỗi text
// //
// // handle:       handle thiết bị
// // boot:         Boot Sector
// // fatTable:     bảng FAT đã nạp
// // firstCluster: cluster đầu tiên của file (lấy từ DirEntry.firstCluster)
// // fileSize:     kích thước thật của file tính bằng byte (lấy từ DirEntry.fileSize)
// // contentOut:   chuỗi sẽ chứa nội dung file sau khi đọc
// //
// // Trả về: true nếu đọc thành công
// //
// // Quan trọng: cluster cuối cùng có thể chứa byte rác sau khi hết nội dung file.
// // Hàm này cắt contentOut theo đúng fileSize sau khi đọc xong.
// // ------------------------------------------------------------
// bool readFileContent(DeviceHandle handle,
//                      const BootSector& boot,
//                      const std::vector<uint32_t>& fatTable,
//                      uint32_t firstCluster,
//                      uint32_t fileSize,
//                      std::string& contentOut);


// // ------------------------------------------------------------
// // Parse nội dung text thành danh sách tiến trình
// //
// // content:    nội dung file .txt đã đọc được
// // processes:  vector sẽ chứa các struct Process sau khi parse
// //
// // Trả về: true nếu parse thành công và có ít nhất 1 tiến trình
// //         false nếu file trống hoặc định dạng không đúng
// //
// // Định dạng file mỗi dòng là một tiến trình:
// //   PID,ArrivalTime,BurstTime,Priority,TimeSlice
// // Dòng đầu tiên có thể là header (bỏ qua nếu không parse được thành số).
// // Dòng trống được bỏ qua.
// // ------------------------------------------------------------
// bool parseProcessList(const std::string& content,
//                       std::vector<Process>& processes);


// // ------------------------------------------------------------
// // Tạo struct FileInfo từ một DirEntry (Chức năng 3 - phần thông tin file)
// //
// // entry:   DirEntry đã parse từ directory.cpp
// // out:     struct FileInfo sẽ được điền dữ liệu
// //
// // Hàm này decode ngày giờ từ dạng packed bit FAT32 sang ngày giờ thông thường.
// // Công thức decode:
// //   year   = ((date >> 9) & 0x7F) + 1980
// //   month  = (date >> 5) & 0x0F
// //   day    =  date & 0x1F
// //   hour   = (time >> 11) & 0x1F
// //   minute = (time >> 5)  & 0x3F
// //   second = (time & 0x1F) * 2       <- FAT lưu giây / 2
// // ------------------------------------------------------------
// void buildFileInfo(const DirEntry& entry, FileInfo& out);


// // ------------------------------------------------------------
// // Kết hợp: đọc file và trả về cả FileInfo lẫn danh sách Process
// // Đây là hàm GUI gọi khi người dùng chọn một file .txt trong danh sách
// //
// // handle:    handle thiết bị
// // boot:      Boot Sector
// // fatTable:  bảng FAT
// // entry:     DirEntry của file .txt được chọn
// // fileInfo:  sẽ được điền thông tin file
// // processes: sẽ được điền danh sách tiến trình
// //
// // Trả về: true nếu cả đọc và parse đều thành công
// // ------------------------------------------------------------
// bool loadTxtFile(DeviceHandle handle,
//                  const BootSector& boot,
//                  const std::vector<uint32_t>& fatTable,
//                  const DirEntry& entry,
//                  FileInfo& fileInfo,
//                  std::vector<Process>& processes);
