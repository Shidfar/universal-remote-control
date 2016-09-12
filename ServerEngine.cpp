//
// Created by Shidfar Hodizoda on 9/7/16.
//

#include "ServerEngine.h"

int mqttCallback(const mosquitto_message* message, void* context);
void *tester(void* context);

ServerEngine::ServerEngine()
{
    this->mqttClient = new MqttClient();
    this->mqttClient->SetCallback(mqttCallback, this);

    this->mySqlAgent = new MySqlAgent();

    pthread_t test_thread;
    int err = pthread_create(&test_thread, NULL, tester, this->mqttClient);
}

int mqttCallback(const mosquitto_message* message, void* context)
{
    ServerEngine *serverEngine = (ServerEngine*) context;
    string topic = string((char*) message->topic);
    string payload = string((char*) message->payload);
    printf("> %s\n> %s\n", topic.c_str(), payload.c_str());
    return 0;
}

void *tester(void* context)
{
    MqttClient* mqttClient = (MqttClient *) context;
    string str;
    while(true)
    {
        getline(cin, str);
        if(str == ".q")
            break;
	if(str == "")
	    continue;
        else
        {
	    cout<<" Sending: " <<str <<endl;
            char topic[] = "/cc3200/";
            int ret = mqttClient->publish(NULL, topic, (int) str.length(), str.c_str(), 1, false);
        }
    }
    return NULL;
}
