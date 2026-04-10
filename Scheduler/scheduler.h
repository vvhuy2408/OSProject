#ifndef scheduler_h
#define scheduler_h
#include "model.h"

// represents a single execution segment in the gantt chart
struct Segment {
    int start;
    int end;
    std::string qID;
    std::string pID;
};

class Scheduler {
private:
    int time = 0;

    std::vector<Process> procs;
    std::vector<SchedulingQueue> queuesList;
    std::vector<Segment> timeline;

    Process* runningProc = nullptr;        // currently running process
    SchedulingQueue* runningQueue = nullptr; // queue that owns the running process
    size_t currentQueueIdx = 0;            // tracks which queue runs next (round robin)
    int queueQuantumUsed = 0;              // time units used in current queue's quantum

    bool isFinished();
    void arrivalCheck();                    
    SchedulingQueue* getQueue(int id);

    Process* selectProcess(SchedulingQueue& q);
    Process* selectSJF(std::vector<Process*>& q);
    Process* selectSRTN(std::vector<Process*>& q);
    void handleSRTNPreempt();

    void runTimeUnit();
    void dispatch();
    void quantumCheck();
    void pushTimeline(int start, int end, Process* p, SchedulingQueue* q);

public:
    Scheduler(std::vector<Process> p, std::vector<SchedulingQueue> q);
    void execute();
    std::vector<Process> getProcesses() { return procs; }
    std::vector<Segment> getTimeline() { return timeline; }
    int getCompletionTime(const std::string& pid) const;
};
#endif