# include <iostream>
# include <vector>
# include <string>
using namespace std;


int main() {
    //============ Read input  ================//
    // read number of frame, in the frist line
    int frame;
    string dummy;
    cin >> dummy >> frame;

    // read page reference string, in the second line
    vector<int> ref;
    int page;
    while (cin >> page) {
        ref.push_back(page);
    }

    //============ output part 1, print the first two lines ================//
    // print page reference string
    for (int v : ref) cout << v << " ";
    cout << endl;
    // print page replacement method
    cout << "Page replacement using LRU" << endl;


    //============ output part 2, record each step ================//
    int time_step = 0;
    int page_fault = 0;
    vector<int> memory;  // the memory frames, which is going to print out, the max size =  number_of_frame
	vector<int> time_of_use;   // track time_of_use order for replacement, invisible from the output

    // iterate through each page reference and check for page faults
    for (int t = 0; t < ref.size(); t++) {
        // current page need to be referenced
        int page = ref[t];
        // print time step
        cout << "time step " << t << ": ";

        // check if page is already in memory
        bool found = false;
        for (int i = 0; i < memory.size(); i++) {
            //------ Case 1: page already in memory ------
            if (memory[i] == page) {
                found = true;
                // update the least-recently-used order
                for (int j = 0; j < time_of_use.size(); j++) {
                    // remove the page from its current position
                    if (time_of_use[j] == page) {
                        time_of_use.erase(time_of_use.begin() + j);
                        break;
                    }
                }
                // move/push the page to the back of the order
                time_of_use.push_back(page);
                break;
            }
        }

        //------ Case 2: page is not in the memory and cause page fault ------
        if (!found) {
            page_fault++;
            // memory not full, simply add the page to memory
            if (memory.size() < frame) {
                memory.push_back(page);
                time_of_use.push_back(page);
            }
            else {
                // remove and update least recently used page from the time_of_use order
                int lru_page = time_of_use[0];
                time_of_use.erase(time_of_use.begin());
                time_of_use.push_back(page);
                // replace the new page to memory at the same position
                for (int i = 0; i < memory.size(); i++) {
                    if (memory[i] == lru_page) { 
                        memory[i] = page;
                        break;
                    }
                }
            }
        }

        // print current memory frames
        for (int i = 0; i < memory.size(); i++) {
            if (i > 0) cout << " ";
            cout << memory[i];
        }
        // move to the next line/frame
        cout << endl;
    }

    //============ output part 3, final result of the page fault ================//
    cout << "total number of page faults = " << page_fault;

    return 0;
}