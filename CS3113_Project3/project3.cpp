
# include <iostream>
# include <vector>
# include <string>
using namespace std;


int main() {
    //============ Read Input ================//
    int resource; // number of resources
    int process;  // number of processes

    // read number of resource types
    string dummy;
    cin >> dummy >> resource;
    // read number of processes
    cin >> dummy >> process;

    // read Available vector
    // contains one value per resource type
    // each value represents the number of available instances for that resource
    string label_available;
    cin >> label_available;
	vector<int> available(resource); 
    for (int i = 0; i < resource; i++){
        cin >> available[i];
    }
    
    // read Max matrix
    // Max is a process x resource matrix; Max[i][j] = maximum demand of resource j by process i
    //  - rows correspond to processes
    //  - columns correspond to resource types
    // read row by row; each row represents one process
    // vector(vector)
    //  - the inner vector contains one entry per resource type (columns of the matrix)
    //  - the outer vector contains one entry/inner per process (rows of the matrix)
    string label_max;
    cin >> label_max;
    vector<vector<int>> max(process, vector<int>(resource));
    for (int i = 0; i < process; i++)
        for (int j = 0; j < resource; j++)
            cin >> max[i][j];

    // read Allocation matrix
    // Allocation is a process x resource matrix; Allocation[i][j] = allocation demand of resource j by process i
    //  - rows correspond to processes
    //  - columns correspond to resource types
    // read row by row; each row represents one process
    // vector(vector)
    //  - the inner vector contains one entry per resource type (columns of the matrix)
    //  - the outer vector contains one entry/inner per process (rows of the matrix)
    string label_allocation;
    cin >> label_allocation;
    vector<vector<int>> allocation(process, vector<int>(resource));
    for (int i = 0; i < process; i++)
        for (int j = 0; j < resource; j++)
            cin >> allocation[i][j];

    // read final request line
    string processID;
    cin >> processID;
    vector<int> request(resource);
    for (int i = 0; i < resource; i++)
        cin >> request[i];

    //============ calculate Need matrix ================//
    // Need is a process x resource matrix; Need[i][j] = need of resource j by process i
    // Need = Max - Allocation
    vector<vector<int>> need(process, vector<int>(resource));
    for (int i = 0; i < process; i++)
        for (int j = 0; j < resource; j++)
            need[i][j] = max[i][j] - allocation[i][j];
    
    //============ the resource-request algorithm ================//
    // check which process is making the request
    // extract process index from processID string
    int pid = stoi(processID.substr(1)); 

    // Step 1: Check if request <= need
    for (int j = 0; j < resource; j++) {
        // if not, state the current system is in unsafe state and exit
        if (request[j] > need[pid][j]) {
            cout << "The current system is in unsafe state." << endl;
            return 0;
        }
    }
    
    // Step 2: Check if request <= Available
    for (int j = 0; j < resource; j++) {
        // if not, state the current system is in unsafe state and exit
        if (request[j] > available[j]) {
            cout << "The current system is in unsafe state." << endl;
            return 0;
        }
    }

    // pass the resource-request algorithm checks
    // Print the state of the system before granting the request
    cout << "Before granting the request of P" << pid << ", the system is in safe state." << endl;
    cout << "Simulating granting P" << pid << "'s request." << endl;
    
    // Step 3: Pretend to allocate requested resources to Pi by modifying the sate
    // Available = Available - Request
    // Allocation = Allocation + Request
    // Need = Need - Request
    for (int j = 0; j < resource; j++) {
        available[j] -= request[j];
        allocation[pid][j] += request[j];
        need[pid][j] -= request[j];
    }

    //print out the updated Need matrix, process x resource
    cout << "New Need" << endl;
    for (int i = 0; i < process; i++) {
        for (int j = 0; j < resource; j++) {
            cout << need[i][j] << " ";
        }
        cout << endl;
    }

    //============ the safety algorithm ================//
    // Check if all the processes can finish with the available resources
    // If yes, then the system is in a safe state and request can be granted.
    // If no, then the system will be in an unsafe state, and the request should be denied (cannot be granted)

    vector<bool> finish(process, false); // initially all processes are unfinished
    vector<int> work = available; // initially work = available
    bool safe = true; // flag to check if we can find a process that can finish

    // while there is potential process that can finish, keep checking for processes
    while (safe){
        safe = false; 
         // start from process 0
        for (int i = 0; i < process; i++){
            // only check unfinished processes
            // if process i is unfinished and its need <= work, it can finish
            if (!finish[i]){
                bool can_finish = true;
                for (int j = 0; j < resource; j++){
                    // if need > work, cannot finish
                    if (need[i][j] > work[j]) {
                        can_finish = false; // mark that this process cannot finish and break the loop. Move to the next process
                        break;
                    }
                }
                // If the process can finish, finish it and then update work
                if (can_finish) {
                    for (int j = 0; j < resource; j++){
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true; // mark this process as finished
                    safe = true; // change the system back to safe state so that we can check again for the next process
                }
            }
        }
    }

    // print out the result
    // if all processes are finished, then request can be granted and the system is in safe state
    // if some processes are unfinished, then request cannot be granted and the system is in unsafe state
    bool all_finished = true;
    for (int i = 0; i < process; i++){
        if (!finish[i]) {
            all_finished = false;
            break;
        }
    }
    if (all_finished) {
        cout << "P" << pid << "'s request can be granted. The system will be in safe state." << endl;
    } else {
        cout << "P" << pid << "'s request cannot be granted. The system will be in unsafe state." << endl;
    }

    return 0;
}
