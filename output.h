#ifndef output_h
#define output_h

#include "model.h"
#include "scheduler.h"
#include <vector>
#include <iostream>
#include <iomanip>

class Output {
public:
    void printReportTerminal(const std::vector<Process>& proc);
    void printGanttChartTerminal(const std::vector<Segment>& timeline);

};

#endif