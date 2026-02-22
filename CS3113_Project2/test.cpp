#include <bits/stdc++.h>
using namespace std;

// Structure to represent a process control block
struct PCB {
    string id;
    int priority;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int last_run_time;      // last time process ran (for RR)
    int remaining_quantum;  // time left in quantum for RR
    int finish_time;
};

// Compare for sorting ready queue
bool comparedReady(PCB* a, PCB* b) {
    // if two process have different priority 
    if (a->priority != b->priority) {
        return a->priority > b->priority; // higher priority first
    }
    // if two process have the same priority 
    return a->last_run_time < b->last_run_time; // RR tie-breaker
}

int main() {
    string line;
    char dummy;
    int time_quantum;
    vector<PCB> processes;

    // Read time quantum
    cin >> dummy >> time_quantum;

    // Read processes
    while (cin >> line) {
        PCB p;
        p.id = line;
        cin >> p.priority >> p.burst_time >> p.arrival_time;
        p.remaining_time = p.burst_time;
        p.last_run_time = -1;
        p.remaining_quantum = time_quantum;
        p.finish_time = 0;
        processes.push_back(p);
    }

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](PCB& a, PCB& b) {
        return a.arrival_time < b.arrival_time;
        });

    int current_time = 0;
    int completed = 0;
    int n = processes.size();
    vector<PCB*> ready;       // ready queue
    PCB* running = nullptr;    // currently running process
    int idle_time = 0;

    while (completed < n) {
        // ==== Add new arrivals to ready queue ====
        for (auto& p : processes) {
            if (p.remaining_time > 0 && p.arrival_time == current_time) {
                ready.push_back(&p);
            }
        }

        // ==== CASE 1: No arrivals, no ready queue, CPU idle ====
        if (ready.empty() && !running) {
            int next_arrival = INT_MAX;
            for (auto& p : processes)
                if (p.remaining_time > 0)
                    next_arrival = min(next_arrival, p.arrival_time);
            int idle_end = next_arrival;
            cout << "Time " << current_time << "-" << idle_end << ": Idle\n";
            idle_time += (idle_end - current_time);
            current_time = idle_end;
            continue;
        }

        // ==== CASE 2,3,4: Decide which process to run ====
        if (!ready.empty() || running) {
            // Combine running process with ready queue for preemption check
            if (running) ready.push_back(running);

            // Sort ready queue by priority & RR order
            sort(ready.begin(), ready.end(), cmpReady);

            PCB* next_run = ready.front();
            ready.erase(ready.begin());

            // Determine runtime
            int run_time = min(next_run->remaining_time, next_run->remaining_quantum);

            // Check if a higher-priority process arrives during this run
            int soonest_higher = INT_MAX;
            for (auto& p : processes) {
                if (p.remaining_time > 0 && p.priority > next_run->priority &&
                    p.arrival_time > current_time && p.arrival_time <= current_time + run_time) {
                    soonest_higher = p.arrival_time - current_time;
                }
            }
            run_time = min(run_time, soonest_higher);

            // ==== Output Gantt chart ====
            cout << "Time " << current_time << "-" << current_time + run_time
                << ": " << next_run->id << " (Priority " << next_run->priority << ")\n";

            // Update process
            next_run->remaining_time -= run_time;
            next_run->remaining_quantum -= run_time;
            next_run->last_run_time = current_time + run_time;
            current_time += run_time;

            // ==== Handle completion ====
            if (next_run->remaining_time == 0) {
                next_run->finish_time = current_time;
                next_run->remaining_quantum = time_quantum;
                completed++;
            }
            // ==== Handle quantum expiration (RR) ====
            else if (next_run->remaining_quantum == 0) {
                next_run->remaining_quantum = time_quantum;
                ready.push_back(next_run); // push to end of RR order
            }
            // Still running process? push back into ready queue
            else {
                running = next_run;
                continue;
            }

            running = nullptr;
        }

    }

    // ==== Calculate Turnaround Time ====
    cout << "Turnaround Time\n";
    for (auto& p : processes) {
        cout << p.id << " = " << p.finish_time - p.arrival_time << "\n";
    }

    // ==== Calculate Waiting Time ====
    cout << "Waiting Time\n";
    for (auto& p : processes) {
        int turnaround = p.finish_time - p.arrival_time;
        int waiting = turnaround - p.burst_time;
        cout << p.id << " = " << waiting << "\n";
    }

    // ==== CPU Utilization ====
    int total_time = 0;
    for (auto& p : processes)
        total_time = max(total_time, p.finish_time);
    cout << "CPU Utilization Time\n";
    cout << (total_time - idle_time) << "/" << total_time << "\n";

    return 0;
}
