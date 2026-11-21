#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

struct Process { int pid, arrival, burst, remaining, level, enqueued; bool arrived; };

int main() {
    const int AGING_THRESHOLD = 3;   
    vector<Process> procs = { {1,0,7,7,1,0,false}, {2,2,1,1,1,0,false} };
    deque<int> Q[4];               
    int time = 0, completed = 0, n = procs.size();

    auto admit = [&]() {
        for (int i = 0; i < n; i++)
            if (!procs[i].arrived && procs[i].arrival <= time) {
                procs[i].arrived = true; procs[i].level = 1; procs[i].enqueued = time;
                Q[1].push_back(i);
            }
    };
    auto applyAging = [&]() {
        for (int lvl = 2; lvl <= 3; lvl++)
            for (size_t k = 0; k < Q[lvl].size(); ) {
                int i = Q[lvl][k];
                if (time - procs[i].enqueued >= AGING_THRESHOLD) {
                    Q[lvl].erase(Q[lvl].begin() + k);
                    procs[i].level = 1; procs[i].enqueued = time;
                    Q[1].push_back(i);
                    cout << "Time " << time << ": Aging trigger -> P" << procs[i].pid << " promoted back to Q1" << endl;
                } else k++;
            }
    };

    while (completed < n) {
        admit();
        applyAging();
        int lvl = !Q[1].empty() ? 1 : !Q[2].empty() ? 2 : !Q[3].empty() ? 3 : 0;
        if (lvl == 0) { time++; continue; }
        int i = Q[lvl].front(); Q[lvl].pop_front();
        Process& p = procs[i];
        int quantum = (lvl == 1) ? 2 : (lvl == 2) ? 4 : p.remaining;
        int run = min(quantum, p.remaining);
        cout << "Time " << time << ": P" << p.pid << " runs in Q" << lvl << " for " << run << " ms";
        time += run; p.remaining -= run;
        if (p.remaining == 0) {
            cout << " -> finishes (t=" << time << ")" << endl;
            completed++;
        } else if (lvl < 3) {
            p.level = lvl + 1; p.enqueued = time; Q[p.level].push_back(i);
            cout << " -> Demoted to Q" << p.level << endl;
        } else { Q[3].push_back(i); cout << endl; }
    }
    cout << "All jobs completed at time " << time << "." << endl;
    return 0;
}
