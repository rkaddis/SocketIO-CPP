#include <iostream>
#include "SocketIO.h"

using namespace std;

int main() {
    TCPListener l("0.0.0.0", 8080);
    if(!l.link()) {
        throw("Failed to connect to client");
    }
    
    while(true) {
        try {
            auto m = l.read();
            cout << "Data: " << m << endl;
        }
        catch(const char* e) {
            std::cout << e << std::endl;
            if(!l.link()) {
                throw("Failed to connect to client");
            }
        }
    }
    return 0;
}