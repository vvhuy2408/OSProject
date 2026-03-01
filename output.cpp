#include "output.h"

void Output::printReportTerminal(const std::vector<Process>& proc) {
    double sumTT = 0;
    double sumWT = 0;
    size_t n = proc.size();

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
    std::cout << "Average Turnaround Time: " << (sumTT / (double)n) << std::endl;
    std::cout << "Average Waiting Time:    " << (sumWT / (double)n) << std::endl;

    std::cout << std::string(62, '=') << std::endl;
}

void Output::printReportToFile(std::ofstream& outFile, const std::vector<Process>& proc) {
    if (!outFile.is_open()) return;
    double sumTT = 0;
    double sumWT = 0;
    size_t n = proc.size();

    outFile << std::string(21, '=') << " Process Statistics " << std::string(21, '=') << std::endl;

    outFile << std::endl << std::left
              << std::setw(10)  << "Process" 
              << std::setw(10) << "Arrival" 
              << std::setw(8)  << "Burst" 
              << std::setw(13) << "Completion" 
              << std::setw(13)  << "Turnaround" 
              << std::setw(13)  << "Waiting" 
              << std::endl;

    outFile << std::string(62, '-') << std::endl;

    for (const auto& p : proc) {
        int TT = p.completionTime - p.arrivalTime;
        int WT = TT - p.burstTime;
        sumTT += TT;
        sumWT += WT;

        outFile << std::left 
                  << std::setw(10)  << p.pID 
                  << std::setw(10) << p.arrivalTime 
                  << std::setw(8)  << p.burstTime 
                  << std::setw(13) << p.completionTime 
                  << std::setw(13)  << TT 
                  << std::setw(13)  << WT << std::endl;    
    }

    outFile << std::string(62, '-') << std::endl;

    outFile << std::fixed << std::setprecision(2);
    outFile << "Average Turnaround Time: " << (sumTT / (double)n) << std::endl;
    outFile << "Average Waiting Time:    " << (sumWT / (double)n) << std::endl;

    outFile << std::string(62, '=') << std::endl;
}

void Output::printGanttChartTerminal(const std::vector<Segment>& tline) {
    if (tline.empty()) return;

    std::vector<Segment> merge;
    merge.push_back(tline[0]);
    for (size_t i = 1; i < tline.size(); i++) {
        if (tline[i].pID == merge.back().pID && tline[i].qID == merge.back().qID) {
            merge.back().end = tline[i].end;
        }
        else merge.push_back(tline[i]);
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

void Output::printGanttChartToFile(std::ofstream& outFile, const std::vector<Segment>& tline) {
    if (!outFile.is_open() || tline.empty()) return;

    std::vector<Segment> merge;
    merge.push_back(tline[0]);
    for (size_t i = 1; i < tline.size(); i++) {
        if (tline[i].pID == merge.back().pID && tline[i].qID == merge.back().qID) {
            merge.back().end = tline[i].end;
        }
        else merge.push_back(tline[i]);
    }

    outFile << std::string(19, '=') << " CPU Scheduling Diagram " << std::string(19, '=') << std::endl;

    outFile << std::endl << std::left
              << std::setw(20)  << "[Start - End]" 
              << std::setw(10) << "Queue" 
              << std::setw(10)  << "Process" 
              << std::endl;

    outFile << std::string(62, '-') << std::endl;

    for (const auto& seg : merge) {
        outFile << std::left << "[" 
                  << std::setw(2) << seg.start << " - " 
                  << std::setw(2) << seg.end 
                  << std::setw(12) << "]" 
                  << std::setw(10) << seg.qID
                  << std::setw(10)  << seg.pID
                  << std::endl;    
    }
    outFile << std::endl;
}
