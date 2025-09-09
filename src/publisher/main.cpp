#include <zmq.h>
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

static bool running = true;
void handle_sigint(int){ running = false; }

int main(int argc, char** argv) {
  std::signal(SIGINT, handle_sigint);

  const char* endpoint = (argc > 1) ? argv[1] : "tcp://*:5556";
  void* ctx = zmq_ctx_new();
  void* pub = zmq_socket(ctx, ZMQ_PUB);
  if (zmq_bind(pub, endpoint) != 0) {
    std::cerr << "bind error: " << zmq_strerror(zmq_errno()) << "\n";
    return 1;
  }
  std::cout << "[PUB] bound at " << endpoint << "\n";

  int counter = 0;
  while (running) {
    const char* topic = "events.temp";
    char payload[128];
    std::snprintf(payload, sizeof(payload), "{\"value\":%d}", counter++);

    zmq_send(pub, topic, std::strlen(topic), ZMQ_SNDMORE);
    zmq_send(pub, payload, std::strlen(payload), 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  zmq_close(pub);
  zmq_ctx_term(ctx);
  return 0;
}
