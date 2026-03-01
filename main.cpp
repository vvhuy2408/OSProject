#include <iostream>
#include <vector>

#include "parser.h"
#include "scheduler.h"
#include "model.h"
#include "output.h"

using namespace std;

int main() {
    vector<SchedulingQueue> qList;
    vector<Process> pList;

    Parser parser;
    parser.readFile("input0.txt", qList, pList);

    Scheduler scheduler(pList, qList);
    scheduler.execute();

    auto proc = scheduler.getProcesses();
    auto tline = scheduler.getTimeline();
    Output output;
    output.printGanttChartTerminal(tline);
    output.printReportTerminal(proc);

    std::ofstream f("output.txt");
    output.printGanttChartToFile(f, tline);
    output.printReportToFile(f, proc);


    // auto timeline = scheduler.getTimeline();
    // auto procs = scheduler.getProcesses();

    // cout << "\n===== CPU SCHEDULING DIAGRAM =====\n";
    // for (auto& s : timeline) {
    //     cout << "[" << s.start << " - " << s.end << "] "
    //          << s.qID << " " << s.pID << "\n";
    // }

    // cout << "\n===== PROCESS STATS =====\n";
    // for (auto& p : procs) {
    //     cout << p.pID
    //          << " | CT=" << p.completionTime
    //          << " | TAT=" << p.turnaroundTime
    //          << " | WT=" << p.waitingTime << "\n";
    // }

    return 0;
}