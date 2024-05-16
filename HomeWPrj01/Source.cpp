#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

mutex coutMutex; 

class Timer {
private:
    chrono::time_point<chrono::high_resolution_clock> start_time;

public:
    void start() {
        start_time = chrono::high_resolution_clock::now();
    }

    double elapsed() const {
        return chrono::duration<double>(chrono::high_resolution_clock::now() - start_time).count();
    }
};

void calculate(int thread_id, int duration) {
    thread::id this_id = this_thread::get_id();
    Timer timer;
    timer.start();
    int progress_length = 50;
    int sleep_duration = duration / progress_length;

    for (int i = 0; i <= progress_length; ++i) {
        {
            lock_guard<mutex> lock(coutMutex);
            stringstream ss;
            ss << "Thread " << setw(2) << thread_id << " (ID: " << this_id << "): [";
            for (int j = 0; j < progress_length; ++j) {
                if (j < i) {
                    ss << "=";
                }
                else {
                    ss << " ";
                }
            }
            ss << "] " << setw(3) << (i * 100 / progress_length) << "%\r";
            cout << ss.str() << flush;
        }
        this_thread::sleep_for(chrono::milliseconds(sleep_duration));
    }

    double elapsed_time = timer.elapsed();
    {
        lock_guard<mutex> lock(coutMutex);
        cout << endl;
        cout << "Thread " << thread_id << ": " << fixed << setprecision(2) << elapsed_time << " seconds" << endl;
    }
}

int main() {
    int num_threads = 3;
    int duration = 5000; 

    vector<thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(calculate, i + 1, duration);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
