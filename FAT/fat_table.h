#pragma once
#include "fat_structs.h"
#include "device.h"
#include <vector>

// ============================================================
// fat_table.h / fat_table.cpp
// TRÁCH NHIỆM: Dev B
// Đọc bảng FAT vào bộ nhớ và tra cứu cluster tiếp theo.
// Đây là nền tảng của mọi thao tác đọc file và thư mục.
// ============================================================


// ------------------------------------------------------------
// Đọc toàn bộ bảng FAT số 1 vào bộ nhớ
//
// handle: handle trả về từ openDevice()
// boot:   struct BootSector đã parse (để biết vị trí và kích thước bảng FAT)
// fatOut: vector sẽ chứa các entry FAT sau khi đọc
//         mỗi phần tử là uint32_t tương ứng với một cluster
//
// Trả về: true nếu đọc thành công
//
// Sau khi gọi hàm này:
//   fatOut[N] & FAT32_MASK = cluster tiếp theo sau cluster N
//   Nếu kết quả >= FAT32_EOC thì cluster N là cuối chuỗi
//
// Lưu ý: Bảng FAT số 1 bắt đầu tại sector reservedSectors.
//        Dev B đọc FAT 1, không cần đọc FAT 2 (FAT 2 là bản sao dự phòng).
// ------------------------------------------------------------
bool loadFATTable(DeviceHandle handle, const BootSector& boot,
                  std::vector<uint32_t>& fatOut);


// ------------------------------------------------------------
// Tra cứu cluster tiếp theo trong chuỗi cluster
//
// fatTable:   bảng FAT đã nạp vào bộ nhớ bởi loadFATTable()
// clusterNum: số thứ tự cluster hiện tại
//
// Trả về: số cluster tiếp theo
//         Nếu trả về >= FAT32_EOC (0x0FFFFFF8) thì đây là cluster cuối
//         Nếu trả về 0 thì cluster đó đang trống (không nên xảy ra khi đọc file)
//
// Ví dụ cách dùng để duyệt cluster chain:
//   uint32_t cluster = firstCluster;
//   while (cluster < FAT32_EOC) {
//       // đọc dữ liệu tại cluster
//       cluster = getNextCluster(fatTable, cluster);
//   }
// ------------------------------------------------------------
uint32_t getNextCluster(const std::vector<uint32_t>& fatTable, uint32_t clusterNum);


// ------------------------------------------------------------
// Kiểm tra một cluster có phải là cluster cuối chuỗi không
//
// clusterValue: giá trị trả về từ getNextCluster()
//
// Trả về: true nếu đây là cluster cuối (>= 0x0FFFFFF8)
// ------------------------------------------------------------
bool isEndOfChain(uint32_t clusterValue);


// ------------------------------------------------------------
// Tính số thứ tự sector đầu tiên của một cluster
//
// clusterNum: số cluster cần tính (bắt đầu từ 2)
// boot:       struct BootSector để lấy các tham số cần thiết
//
// Trả về: số thứ tự sector tuyệt đối trên thiết bị
//
// Công thức:
//   firstDataSector = reservedSectors + (numFATs * fatSize32)
//   sector = firstDataSector + (clusterNum - 2) * sectorsPerCluster
//
// Hàm này được đặt ở đây vì Dev B dùng nhiều nhất,
// nhưng Dev A cũng có thể gọi nếu cần.
// ------------------------------------------------------------
uint64_t clusterToSector(uint32_t clusterNum, const BootSector& boot);
