#ifndef output_h
#define output_h

#include "model.h"
#include "scheduler.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

class Output {
public:
    void printReportTerminal(const std::vector<Process>& proc);
    void printGanttChartTerminal(const std::vector<Segment>& tline);

    void printReportToFile(std::ofstream& outFile, const std::vector<Process>& proc);
    void printGanttChartToFile(std::ofstream& outFile, const std::vector<Segment>& tline);
};

#endif