#pragma once
#include <cstdint>
#include <windows.h>

// ============================================================
// device.h / device.cpp
// truy cập thiết bị raw (USB / thẻ nhớ).
// ============================================================


// ------------------------------------------------------------
// Dùng DeviceHandle thay vì HANDLE trực tiếp trong toàn bộ project
// ------------------------------------------------------------
typedef HANDLE DeviceHandle;
#define INVALID_DEVICE_HANDLE INVALID_HANDLE_VALUE


// ------------------------------------------------------------
// Mở thiết bị để đọc raw
//
// path: đường dẫn thiết bị, ví dụ:
//   "\\\\.\\E:"            - mở theo ký tự ổ đĩa
//   "\\\\.\\PhysicalDrive1" - mở theo số ổ vật lý
//
// Trả về: handle hợp lệ nếu thành công
//         INVALID_DEVICE_HANDLE nếu thất bại
// ------------------------------------------------------------
DeviceHandle openDevice(const char* path);


// ------------------------------------------------------------
// Đóng thiết bị sau khi dùng xong
// ------------------------------------------------------------
void closeDevice(DeviceHandle handle);


// ------------------------------------------------------------
// Đọc một sector vào buffer
//
// sectorNum:  số thứ tự sector cần đọc (bắt đầu từ 0)
// buffer:     con trỏ đến vùng nhớ nhận dữ liệu
// sectorSize: kích thước một sector tính bằng byte
//
// Trả về: true nếu đọc thành công và đọc đủ sectorSize byte
//         false nếu seek thất bại hoặc đọc không đủ byte
// ------------------------------------------------------------
bool readSector(DeviceHandle handle, uint64_t sectorNum, uint8_t* buffer, uint32_t sectorSize);


// ------------------------------------------------------------
// Đọc nhiều sector liên tiếp vào buffer
//
// startSector: sector đầu tiên cần đọc
// count:       số lượng sector cần đọc
// buffer:      vùng nhớ nhận dữ liệu, kích thước
// sectorSize:  kích thước một sector tính bằng byte
//
// Trả về: true nếu đọc thành công toàn bộ count sector
// ------------------------------------------------------------
bool readSectors(DeviceHandle handle, uint64_t startSector, uint32_t count, uint8_t* buffer, uint32_t sectorSize);