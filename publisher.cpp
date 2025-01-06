#include <iostream>
#include "SocketIO.h"
#include <chrono>

using namespace std;

int main() {

    TCPPublisher p;
    p.link("0.0.0.0", 8080);
    
    while(true) {
        auto start = chrono::high_resolution_clock::now();
        try {
            p.send_message("Hello WT!");
            auto elapsed = chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            cout << "Turn Around Time: " << microseconds << "us" << endl; 
            sleep(1);
        }
        catch(const char* e) {
            cout << e << endl;
            p.link("0.0.0.0", 8080, 10000);
        }
    }
    return 0;
}