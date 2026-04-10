#include "device.h"
#include <string>

// // ============================================================
// // device.cpp - Implement tầng truy cập thiết bị raw (Windows)
// // ============================================================


DeviceHandle openDevice(const char* path) {
    // CreateFile với GENERIC_READ: chỉ đọc, không ghi
    // FILE_SHARE_READ | FILE_SHARE_WRITE: cho phép Windows tiếp tục dùng ổ đĩa
    //   (thiếu FILE_SHARE_WRITE thì Windows từ chối vì nó đang mount ổ đó)
    // OPEN_EXISTING: thiết bị phải tồn tại sẵn, không tạo mới
    DeviceHandle handle = CreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    // Nếu thất bại, trả về INVALID_HANDLE_VALUE
    // Caller kiểm tra bằng: if (handle == INVALID_DEVICE_HANDLE)
    if (handle == INVALID_DEVICE_HANDLE) {
        DWORD err = GetLastError();     // 32: device đang bị dùng, 5: access denied, 2: ko tìm thấy ổ
        printf("Failed to open devide: %s\n", path);
        printf("Error code: %lu\n", err);
    }

    return handle;
}


void closeDevice(DeviceHandle handle) {
    if (handle == INVALID_HANDLE_VALUE) {
        return;
    }

    if (!CloseHandle(handle)) {
        printf("Warning: Failed to close handle\n");
    }
}


bool readSector(DeviceHandle handle, uint64_t sectorNum, uint8_t* buffer, uint32_t sectorSize) {
    if (handle == INVALID_DEVICE_HANDLE || buffer == nullptr) {
        return false;
    }

    // Tính byte offset = số sector * kích thước sector
    uint64_t offset = sectorNum * sectorSize;

    // Seek đến đúng vị trí
    // SetFilePointerEx yêu cầu offset là bội số của kích thước sector vật lý
    LARGE_INTEGER li;
    li.QuadPart = offset;
    if (!SetFilePointerEx(handle, li, NULL, FILE_BEGIN)) {
        return false;
    }

    // Đọc đúng sectorSize byte vào buffer
    DWORD bytesRead = 0;
    if (!ReadFile(handle, buffer, sectorSize, &bytesRead, NULL)) {
        return false;
    }

    // Xác nhận đọc đủ số byte yêu cầu
    return (bytesRead == sectorSize);
}


bool readSectors(DeviceHandle handle, uint64_t startSector, uint32_t count,
                 uint8_t* buffer, uint32_t sectorSize) {
    LARGE_INTEGER offset;
    offset.QuadPart = (LONGLONG)startSector * sectorSize;
    if (!SetFilePointerEx(handle, offset, NULL, FILE_BEGIN)) {
        return false;
    }
    DWORD totalBytes = count * sectorSize;
    DWORD bytesRead  = 0;
    if (!ReadFile(handle, buffer, totalBytes, &bytesRead, NULL)) {
        return false;
    }
    return (bytesRead == totalBytes);
}