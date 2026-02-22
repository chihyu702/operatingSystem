#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

// Define the PCB structure (YOU MUST IMPLEMENT THIS)
struct PCB {
    // TODO: Add fields: pid, state, pc, total_work
    int pid;           //Unique process ID
    std::string state; // Process state, like ready, running, or terminated 
    int pc;            //Program Counter
    int totalWork;     //Total number of work units the process requires to complete for each pid

    // TODO: Add constructor PCB
    PCB(int pid_, int work_) {
        pid = pid_;
        totalWork = work_;
        pc = 0;            // count from 0
        state = "Ready";   //the inital state must be ready
    }
};

// Print states of all processes, sorted by PID (PROVIDED - DO NOT MODIFY)
void printProcessStates(const std::vector<PCB>& pcbs, int timeSlice) {
    std::cout << "Interrupt " << timeSlice << ":" << std::endl;
    std::vector<PCB> sorted_pcbs = pcbs;
    std::sort(sorted_pcbs.begin(), sorted_pcbs.end(), 
              [](const PCB& a, const PCB& b) { return a.pid < b.pid; });
    for (const PCB& pcb : sorted_pcbs) {
        std::cout << "PID " << pcb.pid << ": " << pcb.state  << ", at pc " << pcb.pc << std::endl;
    }
    std::cout << std::flush;
}

// TODO: Kernel simulator (YOU MUST IMPLEMENT THIS)
void kernelSimulator(std::vector<PCB>& pcbs, int timeQuantum) {
    //Implement scheduling as decribed in the project 1 description
    //For each timed interrupt call printProcessStates
    //You may create helper functions as needed
    //Add comments to describe your implementation of this function and/or other helper functions
    
    std::queue<int> readyQueue; // ready queue to store all 
    int interrupt = 1; //number of Interrupt

    // initial the ready queue
    for (int i = 0; i < (int)pcbs.size(); i++) {
        readyQueue.push(i);
    }

    // run as long as there are item in ready queue
    while (!readyQueue.empty()) {
        // run the first item in the queue
        int idx = readyQueue.front();
        readyQueue.pop();

        // chage the state to running and working on it
        pcbs[idx].state = "Running";

        // compare the timeQuantum and the number of units still needed for this pid
        // choose the minimum and then it will be the number of units going to work at this slice
        // update pc
        int workThisSlice = std::min(timeQuantum, pcbs[idx].totalWork - pcbs[idx].pc);
        pcbs[idx].pc += workThisSlice;

        // if the pc is really read the total work units needed, then terminate this pid
        if (pcbs[idx].pc >= pcbs[idx].totalWork) {
            pcbs[idx].state = "Terminated";
        }

        // get and print the interrupt time and the state 
        printProcessStates(pcbs, interrupt);
        interrupt++;

        // if it hasn't finish and still in running state, push it at the end of the Ready queue
        if (pcbs[idx].state == "Running") {
            pcbs[idx].state = "Ready";
            readyQueue.push(idx);
        }
    }
}

int main() {
    // Read from standard input via redirection (PROVIDED)
    int numProcesses;
    if (!(std::cin >> numProcesses)) {
        std::cerr << "Error: Invalid input for number of processes" << std::endl;
        return 1;
    }
    if (numProcesses <= 0) {
        std::cerr << "Error: Invalid number of processes" << std::endl;
        return 1;
    }
    
    std::vector<PCB> pcbs;
    std::set<int> pids; // Track unique PIDs
    for (int i = 0; i < numProcesses; ++i) {
        int pid, work;
        if (!(std::cin >> pid >> work)) {
            std::cerr << "Error: Invalid input format for process data" << std::endl;
            return 1;
        }
        if (work <= 0) {
            std::cerr << "Error: Invalid work units for PID " << pid << std::endl;
            return 1;
        }
        if (pids.count(pid) > 0) {
            std::cerr << "Error: Duplicate PID " << pid << " detected" << std::endl;
            return 1;
        }
        // TODO: Add check for unique PIDs (e.g insert pid into the set pids)
        pids.insert(pid);
        // TODO: Create PCB and add to pcbs (e.g., pcbs.emplace_back(pid, work))
        pcbs.emplace_back(PCB(pid, work));
    }
    
    int timeQuantum = 2;
    kernelSimulator(pcbs, timeQuantum);
    
    std::cout << "All processes completed." << std::endl;
    return 0;
}