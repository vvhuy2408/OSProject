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


// bool parseProcessList(const std::string& content,
//                       std::vector<Process>& processes) {
//     processes.clear();
//     std::istringstream stream(content);
//     std::string line;
//     bool firstLine = true;

//     while (std::getline(stream, line)) {
//         // Bỏ qua dòng trống
//         if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
//             continue;
//         }

//         // Thử parse dòng đầu như data, nếu không được thì coi là header và bỏ qua
//         // TODO: Điều chỉnh delimiter (dấu phẩy hoặc dấu cách) theo định dạng file thật của đề bài
//         std::istringstream lineStream(line);
//         std::string token;
//         std::vector<int> values;

//         while (std::getline(lineStream, token, ',')) {
//             try {
//                 // Xóa whitespace ở đầu và cuối token
//                 size_t start = token.find_first_not_of(" \t\r");
//                 size_t end   = token.find_last_not_of(" \t\r");
//                 if (start == std::string::npos) continue;
//                 token = token.substr(start, end - start + 1);

//                 values.push_back(std::stoi(token));
//             } catch (...) {
//                 // Token không phải số - nếu là dòng đầu thì là header, bỏ qua dòng
//                 if (firstLine) {
//                     values.clear();
//                     break;
//                 }
//             }
//         }

//         firstLine = false;

//         // Cần ít nhất PID, ArrivalTime, BurstTime (3 trường tối thiểu)
//         if (values.size() < 3) continue;

//         Process p;
//         p.pid         = values[0];
//         p.arrivalTime = values[1];
//         p.burstTime   = values[2];
//         p.priority    = (values.size() >= 4) ? values[3] : 0;
//         p.timeSlice   = (values.size() >= 5) ? values[4] : 0;

//         processes.push_back(p);
//     }

//     return !processes.empty();
// }


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


// bool loadTxtFile(DeviceHandle handle,
//                  const BootSector& boot,
//                  const std::vector<uint32_t>& fatTable,
//                  const DirEntry& entry,
//                  FileInfo& fileInfo,
//                  std::vector<Process>& processes) {
//     // Bước 1: Điền thông tin file từ entry
//     buildFileInfo(entry, fileInfo);

//     // Bước 2: Đọc nội dung file
//     std::string content;
//     if (!readFileContent(handle, boot, fatTable,
//                          entry.firstCluster, entry.fileSize, content)) {
//         return false;
//     }

//     // Bước 3: Parse danh sách tiến trình
//     if (!parseProcessList(content, processes)) {
//         return false;
//     }

//     return true;
// }
