#pragma once
#include "fat_structs.h"
#include "Scheduler/scheduler.h" // dùng cho segment
#include <vector>
#include <string>

// Khoi tao cua so va ImGui
bool initGUI();

// Vong lap chinh cua GUI, goi den khi nguoi dung dong cua so
// Truyen vao ket qua da doc duoc tu FAT32
void runGUI(const std::string&        devicePath,
            const BootSector&         boot,
            const std::vector<FileInfo>& files);

// Don dep truoc khi thoat
void cleanupGUI();