#ifndef scheduler_h
#define scheduler_h
#include "model.h"

using namespace std;

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

    vector<Process> procs;
    vector<SchedulingQueue> queuesList;
    vector<Segment> timeline;

    Process* runningProc = nullptr;        // currently running process
    SchedulingQueue* runningQueue = nullptr; // queue that owns the running process
    size_t currentQueueIdx = 0;            // tracks which queue runs next (round robin)
    int queueQuantumUsed = 0;              // time units used in current queue's quantum

    bool isFinished();
    void arrivalCheck();                    
    SchedulingQueue* getQueue(int id);

    Process* selectProcess(SchedulingQueue& q);
    Process* selectSJF(vector<Process*>& q);
    Process* selectSRTN(vector<Process*>& q);
    void handleSRTNPreempt();

    void runTimeUnit();
    void dispatch();
    void quantumCheck();
    void pushTimeline(int start, int end, Process* p, SchedulingQueue* q);

public:
    Scheduler(vector<Process> p, vector<SchedulingQueue> q);
    void execute();
    vector<Process> getProcesses() { return procs; }
    vector<Segment> getTimeline() { return timeline; }
};
#endif