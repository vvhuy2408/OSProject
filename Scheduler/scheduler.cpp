#include "scheduler.h"
#include <algorithm>

Scheduler::Scheduler(vector<Process> p, vector<SchedulingQueue> q)
    : procs(p), queuesList(q) {}

// check if all processes are done
bool Scheduler::isFinished() {
    for (auto& p : procs)
        if (!p.completed) return false;
    return true;
}

// add newly arrived processes to their queue
void Scheduler::arrivalCheck() {
    for (auto& p : procs) {
        if (p.arrivalTime == time && !p.completed && p.startTime == -1) {
            SchedulingQueue* q = getQueue(p.curQueueID);
            if (q) q->readyList.push_back(&p);
        }
    }
}

// find queue by id
SchedulingQueue* Scheduler::getQueue(int id) {
    for (auto& q : queuesList)
        if (Process::parseQueueID(q.qID) == id)
            return &q;
    return nullptr;
}

// pick next process to run, skip empty queues
void Scheduler::dispatch() {
    if (runningProc != nullptr) return;

    size_t n = queuesList.size();

    for (size_t i = 0; i < n; i++) {
        size_t idx = (currentQueueIdx + i) % n;
        SchedulingQueue& q = queuesList[idx];

        if (!q.readyList.empty()) {
            currentQueueIdx = idx;

            runningProc = selectProcess(q);

            auto it = find(q.readyList.begin(), q.readyList.end(), runningProc);
            q.readyList.erase(it);

            if (runningProc->startTime == -1)
                runningProc->startTime = time;

            // reset quantum when switching queue
            if (&q != runningQueue)
                queueQuantumUsed = 0;

            runningQueue = &q;
            return;
        }
    }
}

// run 1 time unit
void Scheduler::runTimeUnit() {
    int start = time;

    // no process running, cpu idle
    if (runningProc == nullptr) {
        pushTimeline(start, start + 1, nullptr, nullptr);
        return;
    }

    runningProc->remainingTime--;

    // process finished
    if (runningProc->remainingTime == 0) {
        runningProc->completionTime = time + 1;
        runningProc->completed = true;
        pushTimeline(start, time + 1, runningProc, runningQueue);
        runningProc = nullptr;
        return;
    }

    pushTimeline(start, time + 1, runningProc, runningQueue);
}

// pick shortest job in queue (SJF)
Process* Scheduler::selectSJF(vector<Process*>& q) {
    if (q.empty()) return nullptr;

    size_t idx = 0;
    for (size_t i = 1; i < q.size(); i++) {
        if (q[i]->remainingTime < q[idx]->remainingTime)
            idx = i;
    }
    return q[idx];
}

// SRTN
Process* Scheduler::selectSRTN(vector<Process*>& q) {
    return selectSJF(q);
}

// select process based on queue policy
Process* Scheduler::selectProcess(SchedulingQueue& q) {
    if (q.readyList.empty()) return nullptr;

    if (q.policy == "SJF") return selectSJF(q.readyList);
    if (q.policy == "SRTN") return selectSRTN(q.readyList);

    return q.readyList.front(); // fallback RR
}

// preempt current process if a shorter one arrived (SRTN only)
void Scheduler::handleSRTNPreempt() {
    if (runningProc == nullptr || runningQueue == nullptr) return;
    if (runningQueue->policy != "SRTN") return;
    if (runningQueue->readyList.empty()) return;

    Process* bestProc = selectSRTN(runningQueue->readyList);
    if (bestProc && bestProc->remainingTime < runningProc->remainingTime) {
        runningQueue->readyList.push_back(runningProc);
        auto it = find(runningQueue->readyList.begin(), runningQueue->readyList.end(), bestProc);
        runningQueue->readyList.erase(it);
        runningProc = bestProc;
    }
}

// check if quantum expired, if so move to next queue (RR)
void Scheduler::quantumCheck() {
    if (!runningQueue) return;

    queueQuantumUsed++;

    if (queueQuantumUsed >= runningQueue->timeSlice) {
        // put process back if not done
        if (runningProc != nullptr && runningProc->remainingTime > 0) {
            runningQueue->readyList.push_back(runningProc);
            runningProc = nullptr;
        }

        // move to next queue (round robin)
        currentQueueIdx = (currentQueueIdx + 1) % queuesList.size();
        runningQueue = nullptr;
    }
}

void Scheduler::pushTimeline(int start, int end, Process* p, SchedulingQueue* q) {
    if (p)
        timeline.push_back({start, end, q->qID, p->pID});
    else
        timeline.push_back({start, end, "IDLE", "IDLE"});
}

// main loop
void Scheduler::execute() {
    while (!isFinished()) {
        arrivalCheck();
        handleSRTNPreempt();
        dispatch();
        runTimeUnit();
        quantumCheck();
        time++;
    }
}