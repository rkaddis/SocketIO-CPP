#include <iostream>
#include <cstring>
#include "mqtt/async_client.h"

const std::string SERVER_ADDRESS("mqtt://localhost:1883");
const std::string CLIENT_ID("publisher");
const std::string TOPIC("test");
const int QOS = 1;
const int TIMEOUT = 10000;

using namespace std;

class PublisherCallback : public virtual mqtt::callback
{
public:
    void connection_lost(const std::string& cause) override
    {
        cout << "Connection lost: " << cause << endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override
    {
        cout << "Message delivered" << endl;
    }
};

int main(int argc, char* argv[])
{
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    try
    {
        PublisherCallback callback;
        client.set_callback(callback);

        mqtt::token_ptr connectionToken = client.connect(connOpts);
        connectionToken->wait();

        while (true)
        {
            string message = "hello peer!";

            mqtt::message_ptr pubMessage = mqtt::make_message(TOPIC, message, QOS, false);
            client.publish(pubMessage)->wait();
        }

        mqtt::token_ptr disconnectionToken = client.disconnect();
        disconnectionToken->wait();
    }
    catch (const mqtt::exception& ex)
    {
        cerr << "MQTT Exception: " << ex.what() << endl;
        return 1;
    }

    return 0;
}