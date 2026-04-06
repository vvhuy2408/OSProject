#pragma once
#include "fat_structs.h"
#include "device.h"

// // ============================================================
// // boot_sector.h / boot_sector.cpp
// // TRÁCH NHIỆM: Dev A
// // Chức năng 1: Đọc và parse Boot Sector từ thiết bị raw.
// // ============================================================

// helper function 
static uint16_t readUInt16(const unsigned char* buf, int offset);
static uint32_t readUInt32(const unsigned char* buf, int offset);

// // ------------------------------------------------------------
// // Kiểm tra buffer 512 byte có phải Boot Sector FAT32 hợp lệ không
// //
// // buffer: 512 byte raw đọc từ Sector 0
// //
// // Trả về: true nếu hợp lệ (signature 0x55 0xAA tại offset 510-511
// //         và các giá trị cơ bản không bằng 0)
// //         false nếu không phải FAT32 hoặc bị hỏng
// // ------------------------------------------------------------
bool isValidFAT32(const uint8_t* buffer);

// // ------------------------------------------------------------
// // Đọc Boot Sector từ thiết bị và parse vào struct BootSector
// //
// // handle: handle trả về từ openDevice()
// // out:    con trỏ đến struct BootSector sẽ được điền dữ liệu
// //
// // Trả về: true nếu đọc và parse thành công
// //         false nếu đọc thất bại hoặc signature không hợp lệ
// //
// // Lưu ý: Hàm này đọc 512 byte đầu tiên của thiết bị (Sector 0),
// //        đây luôn là Boot Sector trong FAT32.
// //        Kích thước đọc cố định 512 byte vì tại thời điểm này
// //        chưa biết bytesPerSector (chưa parse xong).
// // ------------------------------------------------------------
bool readBootSector(DeviceHandle handle, BootSector* out);

// // ------------------------------------------------------------
// // Parse buffer 512 byte thành struct BootSector
// //
// // buffer: 512 byte raw đọc từ Sector 0 (đã kiểm tra hợp lệ)
// // out:    struct BootSector sẽ được điền dữ liệu
// //
// // Hàm này chỉ đọc offset và copy dữ liệu, không có logic phức tạp.
// // Tách riêng với readBootSector() để dễ unit test.
// // ------------------------------------------------------------
bool parseBootSector(const uint8_t* buffer, BootSector* out);


// // ------------------------------------------------------------
// // Tính sector bắt đầu của vùng dữ liệu (Data Region)
// //
// // boot: struct BootSector đã parse xong
// //
// // Trả về: số thứ tự sector đầu tiên của vùng dữ liệu
// //
// // Công thức: firstDataSector = reservedSectors + (numFATs * fatSize32)
// // Dev B cũng cần giá trị này để tính offset cluster
// // ------------------------------------------------------------
uint32_t getFirstDataSector(const BootSector& boot);


// // ------------------------------------------------------------
// // In thông tin Boot Sector ra stdout (dùng để debug terminal)
// // GUI sẽ có hàm hiển thị riêng, hàm này chỉ dùng khi test
// // ------------------------------------------------------------
void printBootSector(const BootSector& boot);
