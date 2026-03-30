// #include "fat_table.h"
// #include "boot_sector.h"
// #include <cstring>

// // ============================================================
// // fat_table.cpp - Dev B implement
// // ============================================================


// bool loadFATTable(DeviceHandle handle, const BootSector& boot,
//                   std::vector<uint32_t>& fatOut) {
//     // Bảng FAT 1 bắt đầu ngay sau Reserved Sectors
//     uint64_t fatStartSector = boot.reservedSectors;

//     // Kích thước bảng FAT tính bằng byte
//     uint32_t fatSizeBytes = boot.fatSize32 * boot.bytesPerSector;

//     // Số lượng entry trong bảng FAT (mỗi entry 4 byte)
//     uint32_t entryCount = fatSizeBytes / 4;

//     // Cấp phát buffer tạm để đọc raw bytes
//     std::vector<uint8_t> rawBuffer(fatSizeBytes);

//     if (!readSectors(handle, fatStartSector, boot.fatSize32,
//                      rawBuffer.data(), boot.bytesPerSector)) {
//         return false;
//     }

//     // Copy từ raw bytes sang mảng uint32_t
//     // Mỗi 4 byte liên tiếp = 1 entry FAT
//     fatOut.resize(entryCount);
//     memcpy(fatOut.data(), rawBuffer.data(), fatSizeBytes);

//     return true;
// }


// uint32_t getNextCluster(const std::vector<uint32_t>& fatTable, uint32_t clusterNum) {
//     // Kiểm tra cluster hợp lệ (cluster 0 và 1 là đặc biệt, không dùng cho dữ liệu)
//     if (clusterNum < 2 || clusterNum >= fatTable.size()) {
//         return 0x0FFFFFFF; // Trả về EOC để caller dừng vòng lặp
//     }

//     // Mask 4 bit cao: chỉ lấy 28 bit thấp là giá trị thực sự
//     return fatTable[clusterNum] & FAT32_MASK;
// }


// bool isEndOfChain(uint32_t clusterValue) {
//     // Tất cả giá trị từ 0x0FFFFFF8 trở lên đều là end-of-chain
//     return (clusterValue & FAT32_MASK) >= FAT32_EOC;
// }


// uint64_t clusterToSector(uint32_t clusterNum, const BootSector& boot) {
//     // Sector đầu tiên của vùng dữ liệu
//     uint64_t firstDataSector = getFirstDataSector(boot);

//     // Cluster đánh số từ 2, nên phải trừ 2 trước khi nhân
//     return firstDataSector + ((uint64_t)(clusterNum - 2) * boot.sectorsPerCluster);
// }
