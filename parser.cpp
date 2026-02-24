#include "parser.h"

void Parser::readFile(std::string file, std::vector<SchedulingQueue>& qList, std::vector<Process>& pList) {
    std::ifstream inFile(file);
    if (!inFile.is_open()) {
        std::cerr << file << " not found !!" << std::endl;
        return;
    }
    std::string line;   

    //FIRST LINE - đọc N số lượng queue
    int numQ = 0;   //number of queue
    if (std::getline(inFile, line)) {
        std::stringstream ss(line);
        ss >> numQ;
    }

    //N LINES - đọc thông tin từng queue
    for (int i = 0; i < numQ; i++) {
        if (std::getline(inFile, line)) {
            std::stringstream ss(line);
            std::string qID, policy;
            int timeSlice;

            //vd: Q1  8  SRTN
            if (ss >> qID >> timeSlice >> policy) {
                SchedulingQueue queueN(qID, policy, timeSlice);
                qList.push_back(queueN);
            }
        }
    }

    //PHẦN CÒN LẠI - process
    while (std::getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        Process processN;
        std::string whichQ;

        //vd: P1  0  12  Q1
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


    // //OUTPUT CHECK - để kiểm tra thôi, nào xong hết nhớ xóa
    // std::cout << "Number of queues: " << numQ << std::endl;
    // std::cout << "Queues:" << std::endl;
    // for (const auto& q : qList) {
    //     std::cout << "qID: " << q.qID << " | q = " << q.timeSlice << " | policy: " << q.policy << std::endl;
    // }   
    // std::cout << "Processes:" << std::endl;
    // for (const auto& p : pList) {
    //     std::cout << "pID: " << p.pID << " | aT: " << p.arrivalTime << " | bT: " << p.burstTime << " | which: " << p.curQueueID << std::endl;
    // }
    
}
