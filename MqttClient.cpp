//
// Created by Shidfar Hodizoda on 9/7/16.
//

#include "MqttClient.h"

MqttClient::MqttClient()
{
    loop_stop();
    mosqpp::lib_cleanup();
    this->host = "localhost";
    this->id = MQTTCLIENT;
    this->port = MQTTPORT;
    this->keepalive = 60;
    connect_async(this->host, this->port, this->keepalive);
    loop_start();
}

void MqttClient::on_disconnect(int rc)
{
    printf(" MqttClient Disconnected [%i]\n", rc);
}
void MqttClient::on_connect(int rc)
{
    if ( rc == 0 )
    {
        printf(" MqttClient Connected\n");
        subscribe(0, this->topic, 0);
    }
    else
    {
        printf(" MqttClient connection failed\n");
    }
}
void MqttClient::on_message(const mosquitto_message *message)
{
    this->callback(message, cbcontext);
}
void MqttClient::SetCallback(mqttcallback_fn _callback, void * context)
{
    this->callback = _callback;
    this->cbcontext = context;
}

/*
 *
 * bool MyMosq::send_message(const  char * _message, char * receiver, char *command)
   {
       string myName = EXECUTOR;
       char _topic[512];
       sprintf(_topic, "/pub/1/%s/%s/%s", receiver, EXECUTOR, command);
       int ret = publish(NULL, _topic, (int) strlen(_message),_message,1,false);
       return ( ret == MOSQ_ERR_SUCCESS );
   }
 *
 * */