#include "boot_sector.h"
#include <cstring>
#include <cstdio>

// ============================================================
// boot_sector.cpp
// ============================================================

static uint16_t readUInt16(const unsigned char* buf, int offset) {
    return (uint16_t)buf[offset] | ((uint16_t)buf[offset + 1] << 8);
}

static uint32_t readUInt32(const unsigned char* buf, int offset) {
    return (uint32_t)buf[offset] | ((uint32_t)buf[offset + 1] << 8) | ((uint32_t)buf[offset + 2] << 16) | ((uint32_t)buf[offset + 3] << 24);
}

bool isValidFAT32(const uint8_t* buffer) {
    // Kiểm tra boot signature ở offset 510-511: phải là 0x55 0xAA
    if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
        return false;
    }

    // Kiểm tra các giá trị cơ bản không được bằng 0
    // BytesPerSector tại offset 11 (2 byte)
    uint16_t bytesPerSector = readUInt16(buffer, 11);
    if (bytesPerSector != 512 && bytesPerSector != 1024 && bytesPerSector != 2048 && bytesPerSector != 4096) {
        return false;
    }

    // SectorsPerCluster tại offset 13 (1 byte), FAT32 yêu cầu cluster size theo dạng lũy thừa của 2
    uint8_t sectorsPerCluster = buffer[13];
    if (sectorsPerCluster == 0 || (sectorsPerCluster & (sectorsPerCluster - 1)) != 0) {
        return false;
    }
    
    return true;
}


bool readBootSector(DeviceHandle handle, BootSector* out) {
    if (out == nullptr) {
        return false;
    }

    // Boot Sector luôn nằm ở Sector 0, đọc cố định 512 byte
    uint8_t buffer[512] = {0};

    // Đọc sector 0
    if (!readSector(handle, 0, buffer, 512)) {
        return false;
    }

    // Check signature
    if (!parseBootSector(buffer, out)) {
        return false;
    }

    return true;
}

// // ===== Boot Sector important fields (FAT32) =====
// // BytesPerSector     : offset 11, size 2 bytes
// // SectorsPerCluster  : offset 13, size 1 byte
// // ReservedSectorCount: offset 14, size 2 bytes
// // NumFATs            : offset 16, size 1 byte
// // TotalSectors32     : offset 32, size 4 bytes
// // FATSize32          : offset 36, size 4 bytes
// // RootCluster        : offset 44, size 4 bytes
// // fsInfoSector       : offset 48, size 2 bytes

bool parseBootSector(const uint8_t* buffer, BootSector* out) {
    if (!buffer || !out) return false;

    // Basic FAT32 format validation
    if (!isValidFAT32(buffer)) {
        return false;
    }

    uint8_t numFATs = buffer[16];                       // offset 16 (1bytes)
    uint32_t fatSize32 = readUInt32(buffer, 36);        // offset 36 (4bytes)

    // Logical validation for our program
    if (numFATs == 0 || fatSize32 == 0)
        return false;

    // Đọc từng trường theo offset chuẩn FAT32
    out->bytesPerSector = readUInt16(buffer, 11);       //offset 11 (2byte)    
    out->sectorsPerCluster = buffer[13];                //offset 13 (1byte)    
    out->reservedSectors = readUInt16(buffer, 14);      // offset 14 (2byte)
    out->numFATs = numFATs;                             // buffer[16];    
    out->totalSectors32 = readUInt32(buffer, 32);       // offset 32 (4bytes)
    out->fatSize32 = fatSize32;                         // readUInt32(buffer, 36);    
    out->rootCluster = readUInt32(buffer, 44);          // offset 44 (4bytes)
    out->fsInfoSector = readUInt16(buffer, 48);

    // Text -> Dùng memcpy thay vì ép kiểu con trỏ để tránh vấn đề alignment và padding
    // OEM Name tại offset 3, 8 byte, thêm null terminator
    memcpy(out->oemName, buffer + 3, 8);
    out->oemName[8] = '\0';

    // Volume Label tại offset 71, 11 byte, thêm null terminator
    memcpy(out->volumeLabel, buffer + 71, 11);
    out->volumeLabel[11] = '\0';

    return true;
}


uint32_t getFirstDataSector(const BootSector& boot) {
    // Vùng dữ liệu bắt đầu sau: Reserved Sectors + tất cả bảng FAT
    return boot.reservedSectors + boot.numFATs * boot.fatSize32;
}


void printBootSector(const BootSector& boot) {
    printf("===== BOOT SECTOR INFO =====\n");
    printf("OEM Name                                      : %s\n", boot.oemName);
    printf("Volume Label                                  : %s\n", boot.volumeLabel);

    printf("\n----- SECTOR & CLUSTER INFO -----\n");
    printf("Bytes per Sector                              : %u (0x%04X)\n", boot.bytesPerSector, boot.bytesPerSector);
    // if (boot.bytesPerSector == 512) 
    //     printf("[OK - standard]\n");
    // else if (boot.bytesPerSector == 1024 || boot.bytesPerSector == 2048 || boot.bytesPerSector == 4096) 
    //     printf("[OK]\n");
    // else printf("[WARN - unusual]\n");
    printf("Sectors per Cluster                           : %u (0x%02X)\n", boot.sectorsPerCluster, boot.sectorsPerCluster);
    // if (boot.sectorsPerCluster == 8 || boot.sectorsPerCluster == 16) 
    //     printf("[OK - standard]\n");
    // else printf("[OK - power of 2]\n");
    uint32_t bytesPerCluster = boot.bytesPerSector * boot.sectorsPerCluster;
    printf("Bytes per Cluster                             : %u (0x%08X)\n", bytesPerCluster, bytesPerCluster);

    printf("\n----- FAT & RESERVATION INFO -----\n");
    printf("Number of sectors in the Boot Sector region   : %u (0x%04X)\n", boot.reservedSectors, boot.reservedSectors);
    printf("Number of FAT tables                          : %u (0x%02X)\n", boot.numFATs, boot.numFATs);
    // if (boot.numFATs == 2) 
    //     printf("[OK - standard]\n");
    // else printf("[OK]\n");
    printf("Number of sectors per FAT table               : %u (0x%08X)\n", boot.fatSize32, boot.fatSize32);

    printf("\n----- DIRECTORY & CLUSTER INFO -----\n");
    printf("Root Cluster                                  : %u (0x%08X)\n", boot.rootCluster, boot.rootCluster);
    // if (boot.rootCluster == 2) printf("[OK - standard]\n");
    // else printf("[OK]\n");

    // sectorsPerRDET = clusterCount × sectorsPerCluster
    // printf("Number of sectors for the RDET                : %u (0x%08X)\n", sectorsPerRDET, sectorsPerRDET);

    printf("\n----- TOTAL CAPACITY -----\n");
    printf("Total number of sectors on the disk           : %u (0x%08X)\n", boot.totalSectors32, boot.totalSectors32);
    // printf("FSInfo Sector      : %u (0x%04X)\n", boot.fsInfoSector, boot.fsInfoSector);
    uint32_t firstDataSector = getFirstDataSector(boot);
    printf("First Data Sector                             : %u (0x%08X)\n", firstDataSector, firstDataSector);
    uint64_t totalBytes = (uint64_t)boot.totalSectors32 * boot.bytesPerSector;
    printf("Total Size (GB)                               : %.2f GB\n", totalBytes / (1024.0 * 1024 * 1024));  
}