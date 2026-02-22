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
    std::string curQueueID;
    
    int startTime = -1;     //avoiding confusion with arrivalTime
                            //if startTime =-1, process has not started

    //OUTPUT
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;

    //COMPLETED FLAG CHECK
    bool completed = false;

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