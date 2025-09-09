#include <zmq.h>
#include <csignal>
#include <cstring>
#include <iostream>

static bool running = true;
void handle_sigint(int){ running = false; }

int main(int argc, char** argv) {
  std::signal(SIGINT, handle_sigint);

  const char* endpoint = (argc > 1) ? argv[1] : "tcp://127.0.0.1:5556";
  const char* topic    = (argc > 2) ? argv[2] : "events.temp";

  void* ctx = zmq_ctx_new();
  void* sub = zmq_socket(ctx, ZMQ_SUB);
  if (zmq_connect(sub, endpoint) != 0) {
    std::cerr << "connect error: " << zmq_strerror(zmq_errno()) << "\n";
    return 1;
  }
  zmq_setsockopt(sub, ZMQ_SUBSCRIBE, topic, std::strlen(topic));
  std::cout << "[SUB] connected to " << endpoint << " topic: " << topic << "\n";

  while (running) {
    zmq_msg_t t, p;
    zmq_msg_init(&t);
    zmq_msg_init(&p);

    if (zmq_msg_recv(&t, sub, 0) == -1) break;
    if (zmq_msg_recv(&p, sub, 0) == -1) { zmq_msg_close(&t); break; }

    std::string topicStr(static_cast<char*>(zmq_msg_data(&t)), zmq_msg_size(&t));
    std::string payload(static_cast<char*>(zmq_msg_data(&p)), zmq_msg_size(&p));
    std::cout << "[SUB] " << topicStr << " | " << payload << "\n";

    zmq_msg_close(&t);
    zmq_msg_close(&p);
  }

  zmq_close(sub);
  zmq_ctx_term(ctx);
  return 0;
}
