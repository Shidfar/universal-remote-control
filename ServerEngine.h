//
// Created by Shidfar Hodizoda on 9/7/16.
//

#ifndef UNISERVER_SERVERENGINE_H
#define UNISERVER_SERVERENGINE_H

#include "MqttClient.h"
#include "MySqlAgent.h"
#include "HttpServer.h"

class ServerEngine
{
public:
    ServerEngine();

private:
    MqttClient *mqttClient;
    MySqlAgent *mySqlAgent;
    HttpServer *httpServer;
};


#endif //UNISERVER_SERVERENGINE_H
