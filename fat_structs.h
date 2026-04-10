#ifndef FAT_STRUCTS
#define FAT_STRUCTS
#include <stdint.h>
#include <string>
#include <vector>
#include "Scheduler/model.h"

// ============================================================
// fat_structs.h
// Không chứa bất kỳ logic hay hàm nào - chỉ định nghĩa kiểu dữ liệu.
// ============================================================


// ------------------------------------------------------------
// Thông tin Boot Sector
// ------------------------------------------------------------
struct BootSector {
    uint16_t bytesPerSector;       // Số byte mỗi sector (thường là 512)
    uint8_t  sectorsPerCluster;    // Số sector mỗi cluster (thường là 8 hoặc 16)
    uint16_t reservedSectors;      // Số sector dành riêng (bắt đầu từ sector 0) - Boot sector region
    uint8_t  numFATs;              // Số bảng FAT (thường là 2)

    uint32_t totalSectors32;       // Tổng số sector của toàn bộ phân vùng
    
    uint32_t fatSize32;            // Số sector của một bảng FAT
    uint32_t rootCluster;          // Cluster bắt đầu của Root Directory (thường là 2)
    
    uint16_t fsInfoSector;         // Sector chứa FSInfo (thường là 1)
    char     oemName[9];           // Tên OEM, null-terminated (8 ký tự + null)
    char     volumeLabel[12];      // Nhãn ổ đĩa, null-terminated (11 ký tự + null)
};


// ------------------------------------------------------------
// Thông tin một entry trong bảng thư mục (Directory Entry)
// ------------------------------------------------------------
struct DirEntry {
    char     name[9];              // Tên file 8.3, phần tên (null-terminated)
    char     ext[4];               // Tên file 8.3, phần mở rộng (null-terminated)
    uint8_t  attributes;           // Thuộc tính: 0x10 = thư mục, 0x20 = file thường
    uint32_t firstCluster;         // Cluster đầu tiên chứa dữ liệu file/thư mục
    uint32_t fileSize;             // Kích thước file tính bằng byte (0 nếu là thư mục)
    uint16_t creationTime;         // Thời gian tạo (packed: 5b giờ, 6b phút, 5b giây/2)
    uint16_t creationDate;         // Ngày tạo (packed: 7b năm từ 1980, 4b tháng, 5b ngày)
    std::string fullPath;          // Đường dẫn đầy đủ từ root
};


// ------------------------------------------------------------
// Thông tin hiển thị của một file .txt được chọn - Chức năng 3
// ------------------------------------------------------------
struct FileInfo {
    std::string name;              // Tên file đầy đủ (ví dụ: SCHEDULE.TXT)
    std::string fullPath;          // Đường dẫn đầy đủ từ root
    uint32_t    fileSize;          // Kích thước file tính bằng byte
    int         creationYear;      // Năm tạo file (ví dụ: 2024)
    int         creationMonth;     // Tháng tạo file (1-12)
    int         creationDay;       // Ngày tạo file (1-31)
    int         creationHour;      // Giờ tạo file (0-23)
    int         creationMinute;    // Phút tạo file (0-59)
    int         creationSecond;    // Giây tạo file (0-58, bội số của 2)
    std::vector<SchedulingQueue> queues;    // danh sach hang doi
    std::vector<Process>         processes; // danh sach tien trinh
};

// ------------------------------------------------------------
// Hằng số thuộc tính Directory Entry
// ------------------------------------------------------------
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10       // Entry này là thư mục
#define ATTR_ARCHIVE    0x20       // Entry này là file thông thường
#define ATTR_LFN        0x0F       // Long File Name entry (bỏ qua khi duyệt)

// Giá trị byte đầu của entry cho biết trạng thái
#define ENTRY_FREE      0x00       // Entry trống, không còn entry nào sau đó
#define ENTRY_DELETED   0xE5       // Entry đã bị xóa, bỏ qua

// Giá trị FAT báo hiệu cuối chuỗi cluster
#define FAT32_EOC       0x0FFFFFF8 // End of Cluster chain
#define FAT32_MASK      0x0FFFFFFF // Mask 4 bit cao khi đọc giá trị FAT

#endif