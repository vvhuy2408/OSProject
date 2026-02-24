#ifndef model_h
#define model_h

#include <string>
#include <vector>
#include <queue>

struct Process {
    std::string pID;    //process ID
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;
    int curQueueID;
    
    int startTime = -1;     //avoiding confusion with arrivalTime
                            //if startTime =-1, process has not started

    //OUTPUT
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;

    //COMPLETED FLAG CHECK
    bool completed = false;

    //HELPER PARSE QUEUE
    static int parseQueueID(const std::string& q) {
        if (q.empty()) return 0;

        char prefix = (char)toupper(q[0]);
        if (prefix != 'Q') return 0;

        if (q.size() == 1) return 0;


        for (size_t i = 1; i < q.size(); i++) {
            if (!std::isdigit(q[i])) return 0;
        }

        return std::stoi(q.substr(1));
    }
    //AGING - từ từ cập nhật nếu mún :>
};

struct SchedulingQueue {
    std::string qID;    //queue ID
    std::string policy; //select: rr - sjf - srtn
    int timeSlice;      //rr

    std::vector<Process*> readyList;

    //EMPTY FLAG CHECK
    bool isEmpty() const {
        return readyList.empty();
    }

    SchedulingQueue(std::string id, std::string pol, int qantum = 0) : qID(id), policy(pol), timeSlice(qantum) {}

};

#endif