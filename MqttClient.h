//
// Created by Shidfar Hodizoda on 9/7/16.
//

#ifndef UNISERVER_MQTTCLIENT_H
#define UNISERVER_MQTTCLIENT_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <mosquittopp.h>

#include "GlobalDefs.h"

using namespace std;

typedef int (*mqttcallback_fn)(const mosquitto_message* message, void * context);

class MqttClient : public  mosqpp::mosquittopp
{
public:
    MqttClient();
    void SetCallback(mqttcallback_fn _callback, void * context);
private:
    const char* host;
    const char* id;
    const char* topic;
    int port;
    int keepalive;

    mqttcallback_fn callback;
    void* cbcontext;

    void on_connect(int rc);
    void on_disconnect(int rc);
//    void on_publish(int mid);
    void on_message(const mosquitto_message *message);

};


#endif //UNISERVER_MQTTCLIENT_H
