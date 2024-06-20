#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <unordered_map>
#include <numeric>
#include <climits>
using namespace std;

class Job {
public:
    int id;
    int burst;
    int arrival;
    int remaining;
    int wait;
    int turnaround;

    Job(int id, int burst, int arrival)
        : id(id), burst(burst), arrival(arrival), remaining(burst), wait(0), turnaround(0) {}
};

class Execution {
public:
    int id;
    int start;
    int end;

    Execution(int id, int start, int end)
        : id(id), start(start), end(end) {}
};

void calculateWaitAndTurnaround(vector<Job>& jobs) {
    for (auto& j : jobs) {
        j.turnaround = j.wait + j.burst;
    }
}

void printAverageTimes(vector<Job>& jobs, ofstream& outputFile) {
    int totalWait = 0, totalTurnaround = 0;

    for (const auto& process : jobs) {
        totalWait += process.wait;
        totalTurnaround += process.turnaround;
    }
    long long size = jobs.size();
    outputFile << "Average Waiting Time: " << (float)totalWait / size << endl;
    outputFile << "Average Turnaround Time: " << (float)totalTurnaround / size << endl;
}

bool increasingID(Job& j1, Job& j2) {
    return j1.id < j2.id;
}

void printGanttChart(vector<Execution>& executions, vector<Job>& jobs, ofstream& outputFile) {
    sort(jobs.begin(), jobs.end(), increasingID);
    int totalExecutions = executions.size();
    int totalJobs = jobs.size();
    vector<vector<char>> ganttChart(totalJobs + 1, vector<char>(executions[totalExecutions - 1].end, '-'));
    for(int i=0; i<totalJobs; i++) {
        for(int j = jobs[i].arrival; j<jobs[i].turnaround + jobs[i].arrival; j++) {
            ganttChart[jobs[i].id][j] = '.';
        }
    }
    for(int i=0; i<totalExecutions; i++) {
        for(int j=executions[i].start; j<executions[i].end; j++) {
            ganttChart[executions[i].id][j] = '*';
        }
    }
    outputFile << "Gantt Chart:" << endl;
    outputFile << "       ";
    for(int j=0; j<executions[totalExecutions - 1].end + 1; j++) {
        outputFile << j%10 << " ";
    }
    outputFile << "\n";
    
    for(int i=1; i<totalJobs + 1; i++) {
        outputFile << "J" << jobs[i - 1].id << "     ";
        for(int j=0; j<executions[totalExecutions - 1].end; j++) {
            if(ganttChart[i][j] == '-') {
                outputFile << "| ";
            } else if(ganttChart[i][j] == '.'){
                outputFile << "|.";
            } else {
                outputFile << "|*";
            }
        }
        outputFile << "|\n";
    }
}

bool SortByArrival(Job& a, Job& b){
    return a.arrival < b.arrival;
}

void fcfs(vector<Job> jobs, ofstream& outputFile) {
    vector<Execution> executions;
    int time_marker = 0; 
    
    sort(jobs.begin(), jobs.end(), SortByArrival); 

    for (auto& j : jobs) {
        if (time_marker < j.arrival) {
            time_marker = j.arrival;
        }
        j.wait = max(0, time_marker - j.arrival);
        executions.push_back(Execution(j.id, time_marker, time_marker + j.burst));
        time_marker += j.burst;
    }

    calculateWaitAndTurnaround(jobs);
    outputFile << "FCFS Scheduling:" << endl;
    printAverageTimes(jobs, outputFile);
    printGanttChart(executions, jobs, outputFile);
}

void sjf(vector<Job> jobs, ofstream& outputFile) {
    sort(jobs.begin(), jobs.end(), SortByArrival);

    vector<Execution> executions;
    int currentTime = 0;
    int completedJobs = 0;
    int n = jobs.size();
    vector<bool> isJobCompleted(n, false);

    while (completedJobs < n) {
        int shortestJobIndex = -1;
        int shortestBurstTime = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (jobs[i].arrival <= currentTime && !isJobCompleted[i] && jobs[i].burst < shortestBurstTime) {
                shortestBurstTime = jobs[i].burst;
                shortestJobIndex = i;
            }
        }

        if (shortestJobIndex == -1) {
            currentTime++;
            continue;
        }

        Job& j = jobs[shortestJobIndex];
        j.wait = max(0, currentTime - j.arrival);
        executions.push_back(Execution(j.id, currentTime, currentTime + j.burst));
        currentTime += j.burst;
        j.turnaround = j.wait + j.burst;
        isJobCompleted[shortestJobIndex] = true;
        completedJobs++;
    }

    calculateWaitAndTurnaround(jobs);
    outputFile << "SJF Scheduling:" << endl;
    printAverageTimes(jobs, outputFile);
    printGanttChart(executions, jobs, outputFile);
}

void srtf(vector<Job> jobs, ofstream& outputFile) {
    int n = jobs.size();
    vector<Execution> executions;
    int currentTime = 0;
    int completed = 0;
    int shortestIndex = 0;
    int shortestRemainingTime = INT_MAX;
    bool check = false;

    while (completed != n) {
        for (int i = 0; i < n; ++i) {
            if (jobs[i].arrival <= currentTime && jobs[i].remaining > 0) {
                if (jobs[i].remaining < shortestRemainingTime) {
                    shortestRemainingTime = jobs[i].remaining;
                    shortestIndex = i;
                    check = true;
                }
            }
        }

        if (!check) {
            currentTime++;
            continue;
        }

        executions.push_back(Execution(jobs[shortestIndex].id, currentTime, currentTime + 1));
        jobs[shortestIndex].remaining--;

        shortestRemainingTime = jobs[shortestIndex].remaining;
        if (shortestRemainingTime == 0) {
            shortestRemainingTime = INT_MAX;
        }

        if (jobs[shortestIndex].remaining == 0) {
            completed++;
            check = false;

            jobs[shortestIndex].wait = max(0, currentTime + 1 - jobs[shortestIndex].arrival - jobs[shortestIndex].burst);
            jobs[shortestIndex].turnaround = jobs[shortestIndex].wait + jobs[shortestIndex].burst;
        }

        currentTime++;
    }

    calculateWaitAndTurnaround(jobs);
    outputFile << "SRTF Scheduling:" << endl;
    printAverageTimes(jobs, outputFile);
    printGanttChart(executions, jobs, outputFile);
}

void roundRobin(vector<Job> jobs, int quantum, ofstream& outputFile) {
    vector<Execution> executions;
    int currentTime = 0;
    queue<int> readyQueue;
    int Jobs_completed = 0;

    for (auto& j : jobs) {
        j.remaining = j.burst;
    }

    int n = jobs.size();
    vector<int>arrival(n);
    for (int i = 0; i < n; i++) {
        arrival[i] = jobs[i].arrival;
    }

    while (Jobs_completed != jobs.size()) {
        for (int i = 0; i < n; i++) {
            if (arrival[i] <= currentTime && arrival[i] != -1) {
                readyQueue.push(i);
                arrival[i] = -1;
            }
        }

        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        int current = readyQueue.front();
        readyQueue.pop();

        int timeSlice = min(quantum, jobs[current].remaining);
        executions.push_back(Execution(jobs[current].id, currentTime, currentTime + timeSlice));
        jobs[current].remaining -= timeSlice;
        currentTime += timeSlice;

        for (int i = 0; i < n; i++) {
            if (arrival[i] <= currentTime && arrival[i] != -1) {
                readyQueue.push(i);
                arrival[i] = -1;
            }
        }

        if (jobs[current].remaining > 0) {
            readyQueue.push(current);
        } else {
            Jobs_completed++;
            jobs[current].wait = currentTime - jobs[current].arrival - jobs[current].burst;
        }
    }

    calculateWaitAndTurnaround(jobs);
    outputFile << "Round Robin Scheduling (Quantum = 2):" << endl;
    printAverageTimes(jobs, outputFile);
    printGanttChart(executions, jobs, outputFile);
}

string predictBestAlgorithm(vector<Job>& jobs) {
    float totalBurstTime = accumulate(jobs.begin(), jobs.end(), 0.0f, [](float sum, const Job& j) {
        return sum + j.burst;
    });
    float avgBurstTime = totalBurstTime / jobs.size();

    bool allBurstTimesSame = all_of(jobs.begin(), jobs.end(), [&](const Job& j) {
        return j.burst == jobs[0].burst;
    });

    if (allBurstTimesSame) {
        return "FCFS";
    }

    float burstTimeVariance = 0;
    for (const auto& j : jobs) {
        burstTimeVariance += (j.burst - avgBurstTime) * (j.burst - avgBurstTime);
    }
    burstTimeVariance /= jobs.size();

    float totalArrivalTime = accumulate(jobs.begin(), jobs.end(), 0.0f, [](float sum, const Job& j) {
        return sum + j.arrival;
    });
    float avgArrivalTime = totalArrivalTime / jobs.size();

    float arrivalTimeVariance = 0;
    for (const auto& j : jobs) {
        arrivalTimeVariance += (j.arrival - avgArrivalTime) * (j.arrival - avgArrivalTime);
    }
    arrivalTimeVariance /= jobs.size();

    int longBurstCount = count_if(jobs.begin(), jobs.end(), [&](const Job& j) {
        return j.burst > avgBurstTime;
    });

    if (burstTimeVariance < avgBurstTime && arrivalTimeVariance < avgArrivalTime) {
        return "SJF";
    } else if (burstTimeVariance < avgBurstTime) {
        return "SRTF";
    } else if (longBurstCount > jobs.size() / 2) {
        return "SRTF";
    } else {
        return "SRTF";
    }
}

int main() {
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");

    if (!inputFile.is_open()) {
        cerr << "Error reading input file 🥲" << endl;
        return 0;
    }
    if (!outputFile.is_open()) {
        cerr << "Error reading output file 🥲" << endl;
        return 0;
    }

    vector<Job> jobs;
    int id, burst, arrival;

    while (inputFile >> id >> burst >> arrival) {
        jobs.push_back(Job(id, burst, arrival));
    }

    if (jobs.empty()) {
        outputFile << "No jobs found in input file" << endl;
        return 0;
    } else {
        for (const auto& j : jobs) {
            cerr << "Loaded Job - ID: " << j.id << ", Burst Time: " << j.burst << ", Arrival Time: " << j.arrival << endl;
        }
    }

    string predictedAlgorithm = predictBestAlgorithm(jobs);
    outputFile << "Best algorithm predicted is " << predictedAlgorithm << endl;
    outputFile << endl;

    fcfs(jobs, outputFile);
    sjf(jobs, outputFile);
    srtf(jobs, outputFile);
    roundRobin(jobs, 2, outputFile);

    inputFile.close();
    outputFile.close();
    return 0;
}