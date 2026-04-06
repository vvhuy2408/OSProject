#ifndef parser_h
#define parser_h

#include "model.h"
#include <iostream>
// #include <string>
// #include <vector>
#include <fstream>
#include <sstream>

class Parser {
public:
    void readFile(std::string file, std::vector<SchedulingQueue>& qList, std::vector<Process>& pList);
    void parseFromString(const std::string& content, std::vector<SchedulingQueue>& qList, std::vector<Process>& pList);
};

#endif