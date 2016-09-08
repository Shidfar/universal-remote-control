//
// Created by Shidfar Hodizoda on 9/7/16.
//

#include "ServerEngine.h"

int mqttCallback(const mosquitto_message* message, void* context);

ServerEngine::ServerEngine()
{
    this->mqttClient = new MqttClient();
}

int mqttCallback(const mosquitto_message* message, void* context)
{
    ServerEngine *serverEngine = (ServerEngine*) context;
    string topic = string((char*) message->topic);
    string payload = string((char*) message->payload);
    printf("> %s\n> %s", topic.c_str(), payload.c_str());
}