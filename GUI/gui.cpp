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
                    std::unordered_map<std::string, ImU32> colors;
                    std::vector<ImU32> palette = {
                        IM_COL32(255, 99, 132, 255),
                        IM_COL32(54, 162, 235, 255),
                        IM_COL32(255, 206, 86, 255),
                        IM_COL32(75, 192, 192, 255),
                        IM_COL32(153, 102, 255, 255),
                        IM_COL32(255, 159, 64, 255)
                    };

                    int idx = 0;
                    for (auto& p : f.processes) {
                        colors[p.pID] = palette[idx % palette.size()];
                        idx++;
                    }

                    // scale
                    const float scale = 30.0f;
                    const float blockHeight = 25.0f;
                    const float maxWidth = 650.0f;

                    // Get min/max time
                    int minTime = tl[0].start;
                    int maxTime = tl[0].end;
                    for (auto& seg : tl) {
                        minTime = std::min(minTime, seg.start);
                        maxTime = std::max(maxTime, seg.end);
                    }

                    // Scrollable area for gantt chart
                    ImGui::BeginChild("gantt_scroll", ImVec2(800, (blockHeight + 40) * 2 + 20), 
                                     true, ImGuiWindowFlags_HorizontalScrollbar);

                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
                    float baseX = canvasPos.x + 50;
                    float y = canvasPos.y + 5;

                    // Draw time ticks and grid lines
                    int timeInterval = std::max(1, (maxTime - minTime) / 8);
                    
                    for (int t = minTime; t <= maxTime; t += timeInterval) {
                        float xPos = baseX + (t - minTime) * scale;
                        
                        // Grid line
                        drawList->AddLine(ImVec2(xPos, y - 5), 
                                        ImVec2(xPos, y + blockHeight + 5),
                                        IM_COL32(100, 100, 100, 80), 0.5f);
                        
                        // Time label
                        char buf[16];
                        snprintf(buf, sizeof(buf), "%d", t);
                        drawList->AddText(ImVec2(xPos - 12, y + blockHeight + 5),
                                        IM_COL32(180, 180, 180, 255), buf);
                    }

                    // vẽ timeline blocks
                    for (auto& seg : tl) {
                        float xStart = baseX + (seg.start - minTime) * scale;
                        float width = (seg.end - seg.start) * scale;
                        if (width < 2.0f) width = 2.0f;

                        ImVec2 p1(xStart, y);
                        ImVec2 p2(xStart + width, y + blockHeight);

                        ImU32 color = IM_COL32(200, 200, 200, 255);

                        if (seg.pID != "IDLE" && colors.count(seg.pID))
                            color = colors[seg.pID];

                        drawList->AddRectFilled(p1, p2, color);
                        drawList->AddRect(p1, p2, IM_COL32(0, 0, 0, 255), 1.0f);

                        // PID text
                        ImVec2 textPos(xStart + 3, y + 4);
                        drawList->AddText(textPos, IM_COL32(0, 0, 0, 255), seg.pID.c_str());
                    }

                    ImGui::Dummy(ImVec2((maxTime - minTime) * scale + 100, blockHeight + 40));
                    ImGui::EndChild();
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