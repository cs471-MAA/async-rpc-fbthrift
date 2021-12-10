#include "Utils.h"
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <iostream>

using apache::thrift::RocketClientChannel;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}

uint64_t generate_local_uid(){
    uint64_t ip = 0;
    #ifdef LOCALHOST
        ip = hash<thread::id>()(this_thread::get_id());
    #else
        string sip = exec("hostname -i");
        uint64_t is[4] = {0,0,0,0};
        sscanf(sip.c_str(), "%ld.%ld.%ld.%ld", &(is[3]), &(is[2]), &(is[1]), is);
        ip = (is[3] << 24) | (is[3] << 16) | (is[3] << 8) | is[0];
    #endif
    srand(std::time(nullptr));
    uint64_t random = rand() % 16;
    ip = ((ip << 4) | random ) << 28;
    return ip;
}

uint64_t get_query_uid(uint64_t local_uid, uint64_t query_number){
    return local_uid | ((query_number << 36) >> 36);
}
uint64_t get_epoch_time_us(){
    return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

void sigint_catcher(void (*handler)(int)){
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}