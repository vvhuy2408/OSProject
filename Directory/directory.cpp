#include "directory.h"
#include "fat_table.h"

// ============================================================
// directory.cpp - Dev B implement
// ============================================================


void listAllTxtFiles(DeviceHandle handle,
                     const BootSector& boot,
                     const std::vector<uint32_t>& fatTable,
                     std::vector<DirEntry>& result) {
    // Bắt đầu duyệt từ Root Directory
    // Root Directory nằm tại cluster rootCluster (thường là 2)
    scanDirectory(handle, boot, fatTable, boot.rootCluster, "/", result);
}


void scanDirectory(DeviceHandle handle,
                   const BootSector& boot,
                   const std::vector<uint32_t>& fatTable,
                   uint32_t startCluster,
                   const std::string& currentPath,
                   std::vector<DirEntry>& result) {
    uint32_t currentCluster = startCluster;

    // Duyệt qua toàn bộ cluster chain của thư mục này
    while (!isEndOfChain(currentCluster)) {
        std::vector<std::vector<uint8_t>> rawEntries;

        if (!readDirectoryCluster(handle, boot, currentCluster, rawEntries)) {
            break; // Đọc thất bại, dừng duyệt thư mục này
        }

        for (const auto& rawEntry : rawEntries) {
            // Byte đầu tiên = 0x00: hết entry, không còn gì sau đó
            if (rawEntry[0] == ENTRY_FREE) break;

            // Byte đầu tiên = 0xE5: entry đã xóa, bỏ qua
            if (rawEntry[0] == (uint8_t)ENTRY_DELETED) continue;

            // Bỏ qua Long File Name entry (attribute = 0x0F)
            if (rawEntry[11] == ATTR_LFN) continue;

            DirEntry entry;
            if (!parseDirectoryEntry(rawEntry.data(), currentPath, entry)) {
                continue;
            }

            if (isTxtFile(entry)) {
                // Thêm file .txt vào kết quả
                result.push_back(entry);
            } else if (isSubDirectory(entry)) {
                // Đệ quy vào thư mục con
                std::string subPath = currentPath + std::string(entry.name) + "/";
                scanDirectory(handle, boot, fatTable,
                              entry.firstCluster, subPath, result);
            }
        }

        // Sang cluster tiếp theo trong chuỗi cluster của thư mục
        currentCluster = getNextCluster(fatTable, currentCluster);
    }
}


bool readDirectoryCluster(DeviceHandle handle,
                          const BootSector& boot,
                          uint32_t clusterNum,
                          std::vector<std::vector<uint8_t>>& entriesOut) {
    // Tính kích thước một cluster tính bằng byte
    uint32_t clusterSize = boot.bytesPerSector * boot.sectorsPerCluster;

    // Đọc toàn bộ cluster vào buffer
    std::vector<uint8_t> buffer(clusterSize);
    uint64_t startSector = clusterToSector(clusterNum, boot);

    if (!readSectors(handle, startSector, boot.sectorsPerCluster,
                     buffer.data(), boot.bytesPerSector)) {
        return false;
    }

    // Mỗi entry chiếm đúng 32 byte, tách ra thành từng entry riêng
    uint32_t entryCount = clusterSize / 32;
    entriesOut.resize(entryCount);

    for (uint32_t i = 0; i < entryCount; i++) {
        entriesOut[i].assign(buffer.begin() + i * 32,
                             buffer.begin() + i * 32 + 32);
    }

    return true;
}


bool parseDirectoryEntry(const uint8_t* rawEntry,
                         const std::string& currentPath,
                         DirEntry& out) {
    // Đọc tên file (8 byte) và xóa dấu cách pad ở cuối
    char name[9] = {0};
    memcpy(name, rawEntry + 0, 8);
    // Xóa trailing spaces
    for (int i = 7; i >= 0 && name[i] == ' '; i--) name[i] = '\0';
    memcpy(out.name, name, 9);

    // Đọc phần mở rộng (3 byte) và xóa dấu cách pad ở cuối
    char ext[4] = {0};
    memcpy(ext, rawEntry + 8, 3);
    for (int i = 2; i >= 0 && ext[i] == ' '; i--) ext[i] = '\0';
    memcpy(out.ext, ext, 4);

    // Đọc thuộc tính tại offset 11
    out.attributes = rawEntry[11];

    // Đọc cluster đầu: ghép high word (offset 20) và low word (offset 26)
    uint16_t highWord, lowWord;
    memcpy(&highWord, rawEntry + 20, 2);
    memcpy(&lowWord,  rawEntry + 26, 2);
    out.firstCluster = ((uint32_t)highWord << 16) | lowWord;

    // Đọc kích thước file tại offset 28 (4 byte)
    memcpy(&out.fileSize, rawEntry + 28, 4);

    // Đọc ngày giờ tạo
    memcpy(&out.creationTime, rawEntry + 14, 2);
    memcpy(&out.creationDate, rawEntry + 16, 2);

    // Điền đường dẫn đầy đủ
    out.fullPath = currentPath + std::string(out.name)
                  + (strlen(out.ext) > 0 ? "." + std::string(out.ext) : "");

    return true;
}


bool isTxtFile(const DirEntry& entry) {
    // Không phải thư mục
    if (entry.attributes & ATTR_DIRECTORY) return false;

    // Phần mở rộng phải là "TXT"
    return (strcmp(entry.ext, "TXT") == 0);
}


bool isSubDirectory(const DirEntry& entry) {
    // Phải có attribute thư mục
    if (!(entry.attributes & ATTR_DIRECTORY)) return false;

    // Bỏ qua "." và ".." để tránh đệ quy vô tận
    if (strcmp(entry.name, ".") == 0)  return false;
    if (strcmp(entry.name, "..") == 0) return false;

    return true;
}
