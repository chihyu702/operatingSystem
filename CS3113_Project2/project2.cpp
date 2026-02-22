#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// Structure to represent a process control block
struct PCB {
    string id;
    int priority;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int remaining_quantum;  // quantum left when dong Round robin
    int finish_time;        // time when the process finish
    int last_run_time;      // record the last run time for RR scheduling
};

// Helper funtion compare for sorting ready queue
// Return true if processA going to run first before B
// do priority first and then if same priority, do RR
bool comparedReady(PCB* a, PCB* b) {
    // if two process have different priority 
    if (a->priority != b->priority) {
        // higher priority first
        return a->priority > b->priority; 
    }
    // if two process have the same priority, then do RR
    return a->last_run_time < b->last_run_time;
}

// Helper function record sorted arrival time
// to ensure when the process can be started
bool sortByArrival(const PCB& a, const PCB& b) {
    return a.arrival_time < b.arrival_time;
}

int main() {
    string line;
    char dummy;
    int time_quantum;
    vector<PCB> processes;

    // Read time quantum from standard input
    cin >> dummy >> time_quantum;

    // Read processes from standard input
    while (cin >> line) {
        PCB p;
        p.id = line;
        cin >> p.priority >> p.burst_time >> p.arrival_time;
        p.remaining_time = p.burst_time;     
        p.last_run_time = p.arrival_time;  // Initialize RR time to arrival time
        p.remaining_quantum = time_quantum;
        p.finish_time = 0;
        processes.push_back(p);
    }

    // sort the input by arrival time
    sort(processes.begin(), processes.end(), sortByArrival);

    int current_time = 0;        // count the current time
    int completed = 0;           // record the burst time of the processes that has been completed
    int idle_time = 0;           // record the idle time
    int n = processes.size();    // the size of processes
    vector<PCB*> ready;          // ready queue for the process
    PCB* running = nullptr;      // set the idle running as initial.
    PCB* lastPrinted = nullptr;  // keeps track of the last printed process to merge consecutive Gantt chart segments
    int segmentStart = 0;        // track the segment of each process
    int process_arrival_idx = 0; // track next process to check for arrival

    // ================ scheduling, for different cases  ====================
    // write out and store the gnatt chart for incompleted process
    while (completed < n || running != nullptr || !ready.empty()) {
        // put process into ready queue when it arrives
        // ===== Case 0: if there is nothing runing and new process arrives, put it to ready queue and then run it
        while (process_arrival_idx < n && processes[process_arrival_idx].arrival_time <= current_time) {
            ready.push_back(&processes[process_arrival_idx]);
            process_arrival_idx++;
        }                                                                                               

        // ===== CASE 1: No new arrivals, no ready queue, no process running =====
        // ===== CASE 1: CPU idle run =====
        if (ready.empty() && running == nullptr) {
            int next_arrival = INT_MAX;

            // find the next process arrival time
            if (process_arrival_idx < n) {
                next_arrival = processes[process_arrival_idx].arrival_time;
            }
            else {
                // if there is no next process (all the process have been arrived), then break
                break;
            }

            // print the previous process's execution segment before switching to a new one
            if (lastPrinted != nullptr && current_time > segmentStart) {
                cout << "Time " << segmentStart << "-" << current_time << ": "
                    << lastPrinted->id << " (Priority " << lastPrinted->priority << ")\n";
                lastPrinted = nullptr; // reset lastPrinted
            }

            // print the idle period in the Gantt chart and update total idle time
            cout << "Time " << current_time << "-" << next_arrival << ": Idle\n";
            idle_time += (next_arrival - current_time);
            current_time = next_arrival;

            // add the new arrival to the ready queue immediately
            while (process_arrival_idx < n && processes[process_arrival_idx].arrival_time <= current_time) {
                ready.push_back(&processes[process_arrival_idx]);
                process_arrival_idx++;
            }
            continue;
        }

        // ===== CASE 2: No process arrive, has ready queue. =====
        // ===== CASE 2: select the process from the the ready queue to run =====
        // Decide which process to run next:
        // - Sort the ready queue by priority (and use RR order if priorities are equal)
        // - Pick the first process as the candidate to run
        if ( !ready.empty()) {
            // the first one in the ready queue is the one with highest priority, run it first 
            sort(ready.begin(), ready.end(), comparedReady);
            PCB* top_ready = ready.front();

            // check the preempted
            // if CPU is idle or a higher-priority process is found, preempt the current one.
            if (running == nullptr || top_ready->priority > running->priority) {
                // Update last run time and put the preempted process back into the ready queue
                if (running != nullptr) {
                    running->last_run_time = current_time;
                    ready.push_back(running);
                }
                // if there is no process runing, simply set the selected process as running and remove it from the ready queue
                running = top_ready;
                ready.erase(ready.begin());
            }
        }

        // ===== CASE 3: no process is running, ready queue is empty, and no proces arrives ====
        // ===== keep CPU idle and increment idle time =====
        if (running == nullptr) {
            current_time++;
            continue;
        }

        // CASE 4: process running, new arrival =====
        // CASE 4: decide if it is RR, preemption, or done =====
        // the runtime will be either the quantum time or the remaining_time of the process
        int run_time = min(running->remaining_time, running->remaining_quantum);
        int next_event_time = current_time + run_time;
        bool preempted = false;

        // Check if a higher-priority process arrives during this time
        for (int i = process_arrival_idx; i < n; ++i) {
            PCB* p = &processes[i];
            // if the higher priority happen before the process done
            if (p->remaining_time > 0 &&
                p->priority > running->priority &&
                p->arrival_time < next_event_time) {

                // preempted happen when p->arrival_time
                run_time = p->arrival_time - current_time;
                preempted = true;
                next_event_time = p->arrival_time;
                break;
            }
        }
        

        // === output merging ===
        if (running != lastPrinted) {
            // If previous process was running, close its output line
            if (lastPrinted != nullptr && current_time > segmentStart) {
                cout << "Time " << segmentStart << "-" << current_time << ": "
                    << lastPrinted->id << " (Priority " << lastPrinted->priority << ")\n";
            }
            segmentStart = current_time;
            lastPrinted = running;
        }

        // Run process
        running->remaining_time -= run_time;
        running->remaining_quantum -= run_time;
        current_time = next_event_time;
       

        // Add any new arrivals during execution
        while (process_arrival_idx < n && processes[process_arrival_idx].arrival_time <= current_time) {
            ready.push_back(&processes[process_arrival_idx]);
            process_arrival_idx++;
        }
        if (!ready.empty()) {
            sort(ready.begin(), ready.end(), comparedReady);
        }

        // ===== process done =====
        // change process state after running 
        // === scenario 1: process done ===
        if (running->remaining_time == 0) {
            running->finish_time = current_time;
            running->remaining_quantum = time_quantum; // reset the time quantum
            running->last_run_time = current_time;
            running = nullptr;
            completed++;
        }
        // === scenario 2: do RR, find time quantum === 
        else if (running->remaining_quantum == 0) {
            running->last_run_time = current_time;
            running->remaining_quantum = time_quantum;
            ready.push_back(running);
            running = nullptr;
        }
        else if (preempted) {
            // === scenario 3: preempted by (new arrival) higher priority process ===
            running->last_run_time = current_time;
            running->remaining_quantum = time_quantum;
            ready.push_back(running);
            running = nullptr;
        }
        else if (!ready.empty() && ready.front()->priority > running->priority) {
            // === scenario 4: preempted by higher priority process from the ready queue
            running->last_run_time = current_time;
            running->remaining_quantum = time_quantum;
            ready.push_back(running);
            running = nullptr;
        }
        
        
    }

    // === Handle Gantt chart output merging ===
    if (lastPrinted != nullptr && current_time > segmentStart) {
        cout << "Time " << segmentStart << "-" << current_time << ": "
            << lastPrinted->id << " (Priority " << lastPrinted->priority << ")\n";
    }
        
    // ================ output calculation ====================
     // Turnaround Time
     // process end time - arrival time
    cout << "Turnaround Time\n";
    for (auto& p : processes) {
        int turnaround = p.finish_time - p.arrival_time;
        cout << p.id << " = " << turnaround << "\n";
    }

    // Waiting Time
    // process start time - arrival time
    cout << "Waiting Time\n";
    for (auto& p : processes) {
        int turnaround = p.finish_time - p.arrival_time;
        int waiting = turnaround - p.burst_time;
        cout << p.id << " = " << waiting << "\n";
    }

    // CPU utilization time
    // (total time - idle time)/total time
    int total_time = 0;
    for (auto& p : processes) {
        total_time = max(total_time, p.finish_time);
    }
    cout << "CPU Utilization Time\n";
    cout << (total_time - idle_time) << "/" << total_time << "\n";

    return 0;
}



