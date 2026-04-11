#include "directory.h"
#include "fat_table.h"

// ============================================================
// directory.cpp
// ============================================================

// Moi entry LFN chua 13 ky tu Unicode (UTF-16LE) tai 3 vi tri:
// offset 1: 5 ky tu (10 byte)
// offset 14: 6 ky tu (12 byte)
// offset 28: 2 ky tu  (4 byte)
// Tong 13 ky tu, chi lay byte chan (byte le la 0x00 voi ASCII)
static std::string extractLFNChars(const uint8_t* raw) {
    std::string result;
    int offsets[] = {1, 14, 28};
    int counts[]  = {5,  6,  2};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < counts[i]; j++) {
            uint16_t ch;
            memcpy(&ch, raw + offsets[i] + j * 2, 2);
            if (ch == 0x0000 || ch == 0xFFFF) return result; // het ten
            // Chi xu ly ASCII (ch < 128), bo qua ky tu Unicode phuc tap
            if (ch < 128) result += (char)ch;
            else          result += '?'; // ky tu ngoai ASCII
        }
    }
    return result;
}

// Nhan vao danh sach cac entry LFN (theo thu tu nguoc tu FAT32)
// Tra ve ten day du da ghep lai theo thu tu dung
std::string parseLFN(const std::vector<std::vector<uint8_t>>& lfnEntries) {
    // lfnEntries[0] la entry LFN cuoi cung duoc doc (so thu tu nho nhat)
    // Can dao nguoc lai de ghep ten dung thu tu
    std::string fullName;
    for (int i = (int)lfnEntries.size() - 1; i >= 0; i--) {
        fullName += extractLFNChars(lfnEntries[i].data());
    }
    return fullName;
}

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

        // Buffer gom cac entry LFN lien tiep
        std::vector<std::vector<uint8_t>> lfnBuffer;

        for (const auto& rawEntry : rawEntries) {
            // Byte đầu tiên = 0x00: hết entry, không còn gì sau đó
            if (rawEntry[0] == ENTRY_FREE) break;

            // Byte đầu tiên = 0xE5: entry đã xóa, bỏ qua
            if (rawEntry[0] == (uint8_t)ENTRY_DELETED) {
                lfnBuffer.clear(); // entry bi xoa, bo LFN da gom
                continue;
            }

            // Bỏ qua Long File Name entry (attribute = 0x0F)
            if (rawEntry[11] == ATTR_LFN) {
                // Day la entry LFN, gom vao buffer
                lfnBuffer.push_back(rawEntry);
                continue;
            }

            DirEntry entry;
            if (!parseDirectoryEntry(rawEntry.data(), currentPath, entry)) {
                lfnBuffer.clear();
                continue;
            }
            
            // Neu co LFN thi dung ten LFN thay vi ten 8.3
            if (!lfnBuffer.empty()) {
                entry.lfnName = parseLFN(lfnBuffer);
                // Cap nhat lai fullPath voi ten day du
                entry.fullPath = currentPath + entry.lfnName;
                lfnBuffer.clear();
            }

            if (isTxtFile(entry)) {
                result.push_back(entry);
            } else if (isSubDirectory(entry)) {
                std::string subPath = currentPath
                    + (!entry.lfnName.empty() ? entry.lfnName
                                              : std::string(entry.name))
                    + "/";
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

    // Phần mở rộng phải là txt
    return (strcmp(entry.ext, "TXT") == 0);
}


bool isSubDirectory(const DirEntry& entry) {
    // Phải có attribute thư mục
    if (!(entry.attributes & ATTR_DIRECTORY)) return false;

    // Bỏ qua "." và ".." tránh đệ quy vô tận
    if (strcmp(entry.name, ".") == 0)  return false;
    if (strcmp(entry.name, "..") == 0) return false;

    return true;
}
