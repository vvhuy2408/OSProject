#include "gui.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

static SDL_Window*   gWindow   = nullptr;
static SDL_GLContext gGLContext = nullptr;

bool initGUI() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    // Cau hinh OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    gWindow = SDL_CreateWindow(
        "FAT32 Reader",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!gWindow) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    gGLContext = SDL_GL_CreateContext(gWindow);
    SDL_GL_MakeCurrent(gWindow, gGLContext);
    SDL_GL_SetSwapInterval(1); // vsync

    // Khoi tao ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(gWindow, gGLContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    return true;
}

void runGUI(const std::string&           devicePath,
            const BootSector&            boot,
            const std::vector<FileInfo>& files) {
    bool running = true;
    int  selectedFile = -1; // index file dang duoc chon trong danh sach

    while (running) {
        // Xu ly su kien
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
        }

        // Bat dau frame moi
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // ------------------------------------------------
        // PANEL 1: Boot Sector Info (Chuc nang 1)
        // ------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(420, 720), ImGuiCond_Always);
        ImGui::Begin("Boot Sector Info", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("Device: %s", devicePath.c_str());
        ImGui::Separator();

        if (ImGui::BeginTable("bs_table", 2,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Truong");
            ImGui::TableSetupColumn("Gia tri");
            ImGui::TableHeadersRow();

            auto row = [](const char* label, const char* fmt, auto val) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", label);
                ImGui::TableSetColumnIndex(1); ImGui::Text(fmt, val);
            };

            row("Bytes Per Sector",    "%u", boot.bytesPerSector);
            row("Sectors Per Cluster", "%u", boot.sectorsPerCluster);
            row("Reserved Sectors",    "%u", boot.reservedSectors);
            row("Number of FATs",      "%u", boot.numFATs);
            row("FAT Size (sectors)",  "%u", boot.fatSize32);
            row("Root Cluster",        "%u", boot.rootCluster);
            row("Total Sectors",       "%u", boot.totalSectors32);
            ImGui::EndTable();
        }
        ImGui::End();

        // ------------------------------------------------
        // PANEL 2: Danh sach file .txt (Chuc nang 2)
        // ------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(420, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(420, 360), ImGuiCond_Always);
        ImGui::Begin("Danh sach file .txt", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("Tim thay %zu file:", files.size());
        ImGui::Separator();

        for (int i = 0; i < (int)files.size(); i++) {
            bool selected = (selectedFile == i);
            if (ImGui::Selectable(files[i].fullPath.c_str(), selected))
                selectedFile = i;
        }
        ImGui::End();

        // ------------------------------------------------
        // PANEL 3: Chi tiet file duoc chon (Chuc nang 3)
        // ------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(420, 360), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(860, 360), ImGuiCond_Always);
        ImGui::Begin("Chi tiet file", nullptr,
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (selectedFile >= 0 && selectedFile < (int)files.size()) {
            const FileInfo& f = files[selectedFile];

            // --- Thong tin co ban ---
            ImGui::Text("Ten file  : %s", f.name.c_str());
            ImGui::Text("Duong dan : %s", f.fullPath.c_str());
            ImGui::Text("Kich thuoc: %u bytes", f.fileSize);
            ImGui::Text("Ngay tao  : %04d/%02d/%02d  %02d:%02d:%02d",
                        f.creationYear,  f.creationMonth,  f.creationDay,
                        f.creationHour, f.creationMinute, f.creationSecond);

            ImGui::Separator();

            // --- Bang thong tin tien trinh ---
            if (f.processes.empty()) {
                ImGui::TextDisabled("Khong co du lieu tien trinh.");
            } else {
                ImGui::Text("Danh sach tien trinh (%zu):", f.processes.size());
                ImGui::Spacing();

                if (ImGui::BeginTable("proc_table", 6,
                        ImGuiTableFlags_Borders      |
                        ImGuiTableFlags_RowBg        |
                        ImGuiTableFlags_ScrollY      |
                        ImGuiTableFlags_SizingFixedFit,
                        ImVec2(0, 180))) {

                    // Header
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Process ID");
                    ImGui::TableSetupColumn("Arrival Time");
                    ImGui::TableSetupColumn("CPU Burst Time");
                    ImGui::TableSetupColumn("Priority");
                    ImGui::TableSetupColumn("Queue ID");
                    ImGui::TableSetupColumn("Time Slice");
                    ImGui::TableHeadersRow();

                    // Tim queue tuong ung cua tung tien trinh
                    // de lay Time Slice va Scheduling Algorithm
                    auto findQueue = [&](int queueID) -> const SchedulingQueue* {
                        for (auto& q : f.queues)
                            if (Process::parseQueueID(q.qID) == queueID)
                                return &q;
                        return nullptr;
                    };

                    for (auto& p : f.processes) {
                        const SchedulingQueue* q = findQueue(p.curQueueID);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", p.pID.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", p.arrivalTime);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", p.burstTime);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%d", p.priority);

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("Q%d", p.curQueueID);

                        ImGui::TableSetColumnIndex(5);
                        if (q) ImGui::Text("%d", q->timeSlice);
                        else   ImGui::TextDisabled("-");
                    }

                    ImGui::EndTable();
                }

                // --- Bang thong tin hang doi ---
                ImGui::Spacing();
                ImGui::Text("Hang doi lap lich (%zu):", f.queues.size());
                ImGui::Spacing();

                if (ImGui::BeginTable("queue_table", 3,
                        ImGuiTableFlags_Borders |
                        ImGuiTableFlags_RowBg   |
                        ImGuiTableFlags_SizingFixedFit)) {

                    ImGui::TableSetupColumn("Queue ID");
                    ImGui::TableSetupColumn("Scheduling Algorithm");
                    ImGui::TableSetupColumn("Time Slice");
                    ImGui::TableHeadersRow();

                    for (auto& q : f.queues) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", q.qID.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", q.policy.c_str());

                        ImGui::TableSetColumnIndex(2);
                        if (q.timeSlice > 0)
                            ImGui::Text("%d", q.timeSlice);
                        else
                            ImGui::TextDisabled("-");
                    }

                    ImGui::EndTable();
                }

                // ================= GANTT CHART =================
                ImGui::Separator();
                ImGui::Text("Gantt Chart:");

                // Lay timeline tu file duoc chon
                const std::vector<Segment>& tl = (selectedFile >= 0 && selectedFile < (int)files.size())
                                                  ? files[selectedFile].timeline
                                                  : std::vector<Segment>();

                if (!tl.empty()) {
                    if (ImGui::BeginTable("gantt_chart_table", 3,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_ScrollY,
                            ImVec2(0, 250))) {

                        ImGui::TableSetupScrollFreeze(0, 1);
                        ImGui::TableSetupColumn("Start - End");
                        ImGui::TableSetupColumn("Queue");
                        ImGui::TableSetupColumn("Process");
                        ImGui::TableHeadersRow();

                        for (auto& seg : tl) {
                            ImGui::TableNextRow();
                            
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%d - %d", seg.start, seg.end);
                            
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", seg.qID.c_str());
                            
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", seg.pID.c_str());
                        }

                        ImGui::EndTable();
                    }
                } else {
                    ImGui::TextDisabled("Khong co du lieu timeline.");
                }

                ImGui::Separator();
                ImGui::Text("Turnaround & Waiting Time:");

                if (ImGui::BeginTable("tat_wt_table", 6,
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

                    ImGui::TableSetupColumn("Process");
                    ImGui::TableSetupColumn("Arrival");
                    ImGui::TableSetupColumn("Burst");
                    ImGui::TableSetupColumn("Completion Time");
                    ImGui::TableSetupColumn("Turnaround Time");
                    ImGui::TableSetupColumn("Waiting Time");
                    ImGui::TableHeadersRow();
                    
                    auto getCompletionTime = [&](const std::string& pid) -> int {
                        int completion = -1;

                        for (const auto& seg : tl) {
                            if (seg.pID == pid) {
                                completion = std::max(completion, seg.end);
                            }
                        }

                        return completion;
                    };

                    double totalTAT = 0, totalWT = 0;
                    int processCount = 0;

                    for (auto& p : f.processes) {
                        int completionTime = getCompletionTime(p.pID);

                        int tat = 0;
                        int wt  = 0;

                        if (completionTime != -1) {
                            tat = completionTime - p.arrivalTime;
                            wt  = tat - p.burstTime;
                            totalTAT += tat;
                            totalWT += wt;
                            processCount++;
                        }

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", p.pID.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", p.arrivalTime);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", p.burstTime);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%d", completionTime);

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%d", tat);

                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%d", wt);
                    }

                    // Average row
                    if (processCount > 0) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Average");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextDisabled("-");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextDisabled("-");
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextDisabled("-");
                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%.2f", totalTAT / processCount);
                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%.2f", totalWT / processCount);
                    }

                    ImGui::EndTable();
                }

            }
        } else {
            ImGui::TextDisabled("Chon mot file ben trai de xem chi tiet...");
        }

        ImGui::End();

        // Render
        ImGui::Render();
        int w, h;
        SDL_GetWindowSize(gWindow, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(gWindow);
    }
}

void cleanupGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    if (gGLContext) SDL_GL_DeleteContext(gGLContext);
    if (gWindow)   SDL_DestroyWindow(gWindow);
    SDL_Quit();
}