// #include "boot_sector.h"
// #include <cstring>
// #include <cstdio>

// // ============================================================
// // boot_sector.cpp - Dev A implement
// // ============================================================


// bool readBootSector(DeviceHandle handle, BootSector* out) {
//     // Boot Sector luôn nằm ở Sector 0, đọc cố định 512 byte
//     uint8_t buffer[512] = {0};

//     if (!readSector(handle, 0, buffer, 512)) {
//         return false;
//     }

//     if (!isValidFAT32(buffer)) {
//         return false;
//     }

//     parseBootSector(buffer, out);
//     return true;
// }


// bool isValidFAT32(const uint8_t* buffer) {
//     // Kiểm tra boot signature ở offset 510-511: phải là 0x55 0xAA
//     if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
//         return false;
//     }

//     // Kiểm tra các giá trị cơ bản không được bằng 0
//     // BytesPerSector tại offset 11 (2 byte)
//     uint16_t bytesPerSector;
//     memcpy(&bytesPerSector, buffer + 11, 2);
//     if (bytesPerSector == 0) return false;

//     // SectorsPerCluster tại offset 13 (1 byte), phải là lũy thừa của 2
//     uint8_t sectorsPerCluster = buffer[13];
//     if (sectorsPerCluster == 0) return false;

//     // TODO: Có thể thêm kiểm tra chữ ký "FAT32   " tại offset 82 nếu cần
//     return true;
// }


// void parseBootSector(const uint8_t* buffer, BootSector* out) {
//     // Đọc từng trường theo offset chuẩn FAT32
//     // Dùng memcpy thay vì ép kiểu con trỏ để tránh vấn đề alignment và padding

//     memcpy(&out->bytesPerSector,    buffer + 11, 2);
//     memcpy(&out->sectorsPerCluster, buffer + 13, 1);
//     memcpy(&out->reservedSectors,   buffer + 14, 2);
//     memcpy(&out->numFATs,           buffer + 16, 1);
//     memcpy(&out->totalSectors32,    buffer + 32, 4);
//     memcpy(&out->fatSize32,         buffer + 36, 4);
//     memcpy(&out->rootCluster,       buffer + 44, 4);
//     memcpy(&out->fsInfoSector,      buffer + 48, 2);

//     // OEM Name tại offset 3, 8 byte, thêm null terminator
//     memcpy(out->oemName, buffer + 3, 8);
//     out->oemName[8] = '\0';

//     // Volume Label tại offset 71, 11 byte, thêm null terminator
//     memcpy(out->volumeLabel, buffer + 71, 11);
//     out->volumeLabel[11] = '\0';
// }


// uint32_t getFirstDataSector(const BootSector& boot) {
//     // Vùng dữ liệu bắt đầu sau: Reserved Sectors + tất cả bảng FAT
//     return boot.reservedSectors + (boot.numFATs * boot.fatSize32);
// }


// void printBootSector(const BootSector& boot) {
//     printf("=== Boot Sector ===\n");
//     printf("OEM Name           : %s\n",  boot.oemName);
//     printf("Volume Label       : %s\n",  boot.volumeLabel);
//     printf("Bytes Per Sector   : %u\n",  boot.bytesPerSector);
//     printf("Sectors Per Cluster: %u\n",  boot.sectorsPerCluster);
//     printf("Reserved Sectors   : %u\n",  boot.reservedSectors);
//     printf("Number of FATs     : %u\n",  boot.numFATs);
//     printf("Total Sectors      : %u\n",  boot.totalSectors32);
//     printf("FAT Size (sectors) : %u\n",  boot.fatSize32);
//     printf("Root Cluster       : %u\n",  boot.rootCluster);
//     printf("First Data Sector  : %u\n",  getFirstDataSector(boot));
// }
