#pragma once

#ifndef SOCKETIO_H_
#define SOCKETIO_H_

#define MSG_SIZE 1024


#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <chrono>
#include <map>


using namespace std;

class TCPMessage {

    private:
        map<string, double> dict;

    public:
        TCPMessage(map<string, double> data) {
            this->dict = data;
        }

        map<string,double> data() {
            return this->dict;
        }

        static string toString(map<string,double> dict) {
            string out = "";
            for(auto pair : dict) {
                out += pair.first + "," + to_string(pair.second) + ";";
            }
            return out;
        }

        static map<string,double> toMap(string dictString) {
            map<string,double> out;
            string nameBuffer = "";
            string valBuffer = "";
            bool nameReadFlag = false;
            for(const char c : dictString) {
                if(c == 0) { //null character for padding
                    break;
                }
                if(!nameReadFlag) {
                    if(c == ',') {
                        nameReadFlag = !nameReadFlag;
                    }
                    else {
                        nameBuffer += c;
                    }
                }
                else {
                    if(c == ';') {
                        out[nameBuffer] = stod(valBuffer);
                        nameReadFlag = !nameReadFlag;
                        nameBuffer = "";
                        valBuffer = "";
                    }
                    else {
                        valBuffer += c;
                    }
                }
                
            }
            return out;
        }




};

class TCPListener {

    private:
        const char* host_ip;
        int port_number;
        bool is_bound;

        int listen_socket;

        int client = -1;
        char* client_ip;
        int client_port;

        

    public:

        TCPListener() {
            this->is_bound = false;
        }

        TCPListener(const char* host_ip, int port_number) {
            this->is_bound = false;
            bind_port(host_ip, port_number);
        }

        

        void bind_port(const char* host_ip, int port_number) {
            if(is_bound) { return; }

            this->host_ip = host_ip;
            this->port_number = port_number;

            this->listen_socket = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(port_number); //port number
            serverAddress.sin_addr.s_addr = inet_addr(host_ip); //ip address

            // binding socket.
            bind(this->listen_socket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
            listen(this->listen_socket, 5);
            this->is_bound = true;
        }

        bool link() {
            if (!this->is_bound) { throw("Socket is not bound!"); }
            this->client = accept(this->listen_socket, nullptr, nullptr);
            return this->client > 0;
        }

        char* read() {
            
            if(this->client <= -1) { throw("No client has connected! Run link() first."); }
            
            char* buffer = new char[MSG_SIZE];
            int readbytes = 0;
            readbytes = recv(client, buffer, sizeof(buffer), 0);
            if(readbytes <= 0) {
                throw("Connection to sender has been terminated by peer.");
            }
            return buffer;
        }

        void release() {
            close(this->listen_socket);
            this->is_bound = false;
        }

        ~TCPListener() {
            release();
        }

};

class TCPPublisher {
    private:
        const char* dest_ip;
        int dest_port = -1;
        int dest_socket;
        bool is_connected;

        static void handle_sigpipe(int signum) {
            throw("Connection to receiver has been terminated by peer.");
        }

    public:

        TCPPublisher() {
            this->is_connected = false;
        }

        void link(const char* dest_ip, int dest_port, int timeout_ms=-1) {
            this->dest_ip = dest_ip;
            this->dest_port = dest_port;
            this->dest_socket = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(dest_port);
            serverAddress.sin_addr.s_addr = inet_addr(dest_ip);
            auto timeout = chrono::high_resolution_clock::now() + chrono::milliseconds(timeout_ms);
            int connect_ret = -1;
            while(connect_ret != 0) {
                connect_ret = connect(this->dest_socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
                if(timeout_ms != -1 && chrono::high_resolution_clock::now() >= timeout) {
                    throw("Connection to receiver timed out.");
                }
            }
            
            this->is_connected = true;
            cout << "Publisher has successfully connected to destination" << endl;
        }

        void send_message(char* data) {
            if(!this->is_connected) { 
                throw("The host is not connected! Run link() first."); 
            }
            
            int bytes_sent = send(this->dest_socket, data, sizeof(data), MSG_NOSIGNAL);
            if(bytes_sent <= 0) {
                throw("Unable to send data to peer. Maybe the peer is closing/closed?");
            }
        }

        void release() {
            if(this->is_connected) {
                close(this->dest_socket);
            }
        }

        ~TCPPublisher() {
            release();
        }
};







#endif