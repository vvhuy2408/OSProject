#include "file_reader.h"
#include "fat_table.h"
#include <sstream>
#include <cstring>

// ============================================================
// file_reader.cpp - Dev B implement
// ============================================================

bool readFileContent(DeviceHandle handle,
                     const BootSector& boot,
                     const std::vector<uint32_t>& fatTable,
                     uint32_t firstCluster,
                     uint32_t fileSize,
                     std::string& contentOut) {
    if (fileSize == 0) {
        contentOut = "";
        return true;
    }

    uint32_t clusterSize = boot.bytesPerSector * boot.sectorsPerCluster;
    std::vector<uint8_t> rawContent;
    rawContent.reserve(fileSize);

    uint32_t currentCluster = firstCluster;

    // Đọc từng cluster trong chuỗi cluster của file
    while (!isEndOfChain(currentCluster)) {
        std::vector<uint8_t> clusterBuf(clusterSize);
        uint64_t startSector = clusterToSector(currentCluster, boot);

        if (!readSectors(handle, startSector, boot.sectorsPerCluster,
                         clusterBuf.data(), boot.bytesPerSector)) {
            return false;
        }

        // Thêm dữ liệu cluster này vào buffer tổng
        rawContent.insert(rawContent.end(), clusterBuf.begin(), clusterBuf.end());

        currentCluster = getNextCluster(fatTable, currentCluster);
    }

    // Cắt theo fileSize thực sự để bỏ byte rác ở cluster cuối
    if (rawContent.size() > fileSize) {
        rawContent.resize(fileSize);
    }

    contentOut = std::string(rawContent.begin(), rawContent.end());
    return true;
}

void buildFileInfo(const DirEntry& entry, FileInfo& out) {
    // Neu co ten LFN thi dung, khong thi dung ten 8.3
    if (!entry.lfnName.empty())
        out.name = entry.lfnName;
    else
        out.name = std::string(entry.name) + "." + std::string(entry.ext);
        
    out.fullPath = entry.fullPath;
    out.fileSize = entry.fileSize;

    // Decode ngày từ packed bit FAT32
    // Bit layout: [15:9] = năm (từ 1980), [8:5] = tháng, [4:0] = ngày
    uint16_t date = entry.creationDate;
    out.creationYear  = ((date >> 9) & 0x7F) + 1980;
    out.creationMonth =  (date >> 5) & 0x0F;
    out.creationDay   =   date & 0x1F;

    // Decode giờ từ packed bit FAT32
    // Bit layout: [15:11] = giờ, [10:5] = phút, [4:0] = giây/2
    uint16_t time = entry.creationTime;
    out.creationHour   = (time >> 11) & 0x1F;
    out.creationMinute = (time >> 5)  & 0x3F;
    out.creationSecond = (time & 0x1F) * 2; // FAT lưu giây chia 2
}