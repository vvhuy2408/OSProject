// #include "device.h"
// #include <string>

// // ============================================================
// // device.cpp - Implement tầng truy cập thiết bị raw (Windows)
// // ============================================================


// DeviceHandle openDevice(const char* path) {
//     // CreateFile với GENERIC_READ: chỉ đọc, không ghi
//     // FILE_SHARE_READ | FILE_SHARE_WRITE: cho phép Windows tiếp tục dùng ổ đĩa
//     //   (thiếu FILE_SHARE_WRITE thì Windows từ chối vì nó đang mount ổ đó)
//     // OPEN_EXISTING: thiết bị phải tồn tại sẵn, không tạo mới
//     DeviceHandle handle = CreateFileA(
//         path,
//         GENERIC_READ,
//         FILE_SHARE_READ | FILE_SHARE_WRITE,
//         NULL,
//         OPEN_EXISTING,
//         0,
//         NULL
//     );

//     // Nếu thất bại, trả về INVALID_HANDLE_VALUE
//     // Caller kiểm tra bằng: if (handle == INVALID_DEVICE_HANDLE)
//     return handle;
// }


// bool readSector(DeviceHandle handle, uint64_t sectorNum, uint8_t* buffer, uint32_t sectorSize) {
//     // Tính byte offset = số sector * kích thước sector
//     LARGE_INTEGER offset;
//     offset.QuadPart = (LONGLONG)sectorNum * sectorSize;

//     // Seek đến đúng vị trí
//     // SetFilePointerEx yêu cầu offset là bội số của kích thước sector vật lý
//     if (!SetFilePointerEx(handle, offset, NULL, FILE_BEGIN)) {
//         return false;
//     }

//     // Đọc đúng sectorSize byte vào buffer
//     DWORD bytesRead = 0;
//     if (!ReadFile(handle, buffer, sectorSize, &bytesRead, NULL)) {
//         return false;
//     }

//     // Xác nhận đọc đủ số byte yêu cầu
//     return (bytesRead == sectorSize);
// }


// bool readSectors(DeviceHandle handle, uint64_t startSector, uint32_t count,
//                  uint8_t* buffer, uint32_t sectorSize) {
//     LARGE_INTEGER offset;
//     offset.QuadPart = (LONGLONG)startSector * sectorSize;

//     if (!SetFilePointerEx(handle, offset, NULL, FILE_BEGIN)) {
//         return false;
//     }

//     DWORD totalBytes = count * sectorSize;
//     DWORD bytesRead  = 0;
//     if (!ReadFile(handle, buffer, totalBytes, &bytesRead, NULL)) {
//         return false;
//     }

//     return (bytesRead == totalBytes);
// }


// void closeDevice(DeviceHandle handle) {
//     if (handle != INVALID_HANDLE_VALUE) {
//         CloseHandle(handle);
//     }
// }


// std::string getLastErrorMessage() {
//     DWORD errorCode = GetLastError();
//     if (errorCode == 0) return "No error";

//     LPSTR messageBuffer = nullptr;
//     FormatMessageA(
//         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//         NULL,
//         errorCode,
//         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//         (LPSTR)&messageBuffer,
//         0,
//         NULL
//     );

//     std::string message(messageBuffer ? messageBuffer : "Unknown error");
//     LocalFree(messageBuffer);

//     // Xóa ký tự newline thừa ở cuối mà FormatMessage thêm vào
//     while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
//         message.pop_back();

//     return message;
// }
