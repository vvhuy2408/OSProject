#include "parser.h"

void Parser::readFile(std::string file, std::vector<SchedulingQueue>& qList, std::vector<Process>& pList) {
    std::ifstream inFile(file);
    if (!inFile.is_open()) {
        std::cerr << file << " not found !!" << std::endl;
        return;
    }
    std::string line;

    // first line: number of queues
    int numQ = 0;
    if (std::getline(inFile, line)) {
        std::stringstream ss(line);
        ss >> numQ;
    }

    // next N lines: queue info 
    for (int i = 0; i < numQ; i++) {
        if (std::getline(inFile, line)) {
            std::stringstream ss(line);
            std::string qID, policy;
            int timeSlice;

            // (e.g. Q1 8 SRTN)
            if (ss >> qID >> timeSlice >> policy) {
                SchedulingQueue queueN(qID, policy, timeSlice);
                qList.push_back(queueN);
            }
        }
    }

    // remaining lines: process info 
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        Process processN;
        std::string whichQ;

        // (e.g. P1 0 12 Q1)
        if (ss >> processN.pID >> processN.arrivalTime >> processN.burstTime >> whichQ) {
            processN.remainingTime = processN.burstTime;
            processN.curQueueID = Process::parseQueueID(whichQ);
            processN.startTime = -1;
            processN.completed = false;
            pList.push_back(processN);
        }
    }

    inFile.close();
    std::cout << "File read successfully!" << std::endl;
}