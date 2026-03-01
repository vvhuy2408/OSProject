#ifndef output_h
#define output_h

#include "model.h"
#include "scheduler.h"
#include <vector>
#include <iostream>
#include <iomanip>

class Output {
public:
    void printGanttChart();
    void printReport(const std::vector<Process>& proc);
};

#endif