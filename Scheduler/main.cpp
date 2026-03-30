#include <iostream>
#include <vector>

#include "parser.h"
#include "scheduler.h"
#include "model.h"
#include "output.h"

using namespace std;

int main(int argc, char* argv[]) {
    // need exactly 2 args: input and output file
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " input.txt output.txt" << endl;
        return 1;
    }

    vector<SchedulingQueue> qList;
    vector<Process> pList;

    // parse input file
    Parser parser;
    parser.readFile(argv[1], qList, pList);

    // run the scheduler
    Scheduler scheduler(pList, qList);
    scheduler.execute();

    // print results to terminal
    auto proc = scheduler.getProcesses();
    auto tline = scheduler.getTimeline();
    Output output;
    output.printGanttChartTerminal(tline);
    output.printReportTerminal(proc);

    // write results to output file
    std::ofstream f(argv[2]);
    if (!f.is_open()) {
        cerr << "Cannot open output file: " << argv[2] << endl;
        return 1;
    }
    output.printGanttChartToFile(f, tline);
    output.printReportToFile(f, proc);

    return 0;
}