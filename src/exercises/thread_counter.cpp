#include <chrono>
#include <csignal>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

static bool running = true;
static std::mutex mtx;
static long long counter = 0;

void handle_sigint(int) { running = false; }

void worker(int id) {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            ++counter;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10 + id * 5));
    }
}

int main() {
    std::signal(SIGINT, handle_sigint);
    std::cout << "[thread_counter] start (Ctrl+C para parar)\n";

    const int N = 3; // 3 hilos
    std::vector<std::thread> ths;
    ths.reserve(N);
    for (int i = 0; i < N; ++i) ths.emplace_back(worker, i);

    // monitoriza 5 segundos
    for (int s = 0; s < 5; ++s) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        long long snap;
        {
            std::lock_guard<std::mutex> lock(mtx);
            snap = counter;
        }
        std::cout << "[thread_counter] t=" << (s + 1) << "s counter=" << snap << "\n";
    }

    running = false;
    for (auto &t : ths) t.join();

    std::cout << "[thread_counter] final counter=" << counter << "\n";
    return 0;
}
