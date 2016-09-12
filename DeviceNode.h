//
// Created by Shidfar Hodizoda on 9/12/16.
//

#ifndef UNISERVER_DEVICENODE_H
#define UNISERVER_DEVICENODE_H

#include "MqttClient.h"

class DeviceNode
{
public:
    string name;
    MqttClient* mqttClient;

    DeviceNode();
    void set_mqttclient(MqttClient *_mqttClient);
};


#endif //UNISERVER_DEVICENODE_H
