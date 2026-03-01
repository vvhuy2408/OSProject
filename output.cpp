#include "output.h"

void Output::printReportTerminal(const std::vector<Process>& proc) {
    double sumTT = 0;
    double sumWT = 0;
    int n = proc.size();

    std::cout << std::string(21, '=') << " Process Statistics " << std::string(21, '=') << std::endl;

    std::cout << std::endl << std::left
              << std::setw(10)  << "Process" 
              << std::setw(10) << "Arrival" 
              << std::setw(8)  << "Burst" 
              << std::setw(13) << "Completion" 
              << std::setw(13)  << "Turnaround" 
              << std::setw(13)  << "Waiting" 
              << std::endl;

    std::cout << std::string(62, '-') << std::endl;

    for (const auto& p : proc) {
        int TT = p.completionTime - p.arrivalTime;
        int WT = TT - p.burstTime;
        sumTT += TT;
        sumWT += WT;

        std::cout << std::left 
                  << std::setw(10)  << p.pID 
                  << std::setw(10) << p.arrivalTime 
                  << std::setw(8)  << p.burstTime 
                  << std::setw(13) << p.completionTime 
                  << std::setw(13)  << TT 
                  << std::setw(13)  << WT << std::endl;    
    }

    std::cout << std::string(62, '-') << std::endl;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Turnaround Time: " << (sumTT / n) << std::endl;
    std::cout << "Average Waiting Time:    " << (sumWT / n) << std::endl;

    std::cout << std::string(62, '=') << std::endl;
}

void Output::printGanttChartTerminal(const std::vector<Segment>& timeline) {
    if (timeline.empty()) return;

    std::vector<Segment> merge;
    merge.push_back(timeline[0]);
    for (int i = 1; i < timeline.size(); i++) {
        if (timeline[i].pID == merge.back().pID && timeline[i].qID == merge.back().qID) {
            merge.back().end = timeline[i].end;
        }
        else merge.push_back(timeline[i]);
    }

    std::cout << std::string(19, '=') << " CPU Scheduling Diagram " << std::string(19, '=') << std::endl;

    std::cout << std::endl << std::left
              << std::setw(20)  << "[Start - End]" 
              << std::setw(10) << "Queue" 
              << std::setw(10)  << "Process" 
              << std::endl;

    std::cout << std::string(62, '-') << std::endl;

    for (const auto& seg : merge) {
        std::cout << std::left << "[" 
                  << std::setw(2) << seg.start << " - " 
                  << std::setw(2) << seg.end 
                  << std::setw(12) << "]" 
                  << std::setw(10) << seg.qID
                  << std::setw(10)  << seg.pID
                  << std::endl;    
    }
    std::cout << std::endl;
}
